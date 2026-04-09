#if defined(SOKOL_IMPL) && !defined(SOKOL_GFX_UTILS_IMPL)
#define SOKOL_GFX_UTILS_IMPL
#endif
#ifndef SOKOL_GFX_UTILS_INCLUDED
#define SOKOL_GFX_UTILS_INCLUDED

/*
    sokol_gfx_utils.h -- readback support for sokol_gfx.h

    Project URL: https://github.com/floooh/sokol

    Include this header *after* the sokol_gfx.h implementation to access
    internal state.

    Do this:
        #define SOKOL_IMPL or
        #define SOKOL_GFX_UTILS_IMPL
    before you include this file in *one* C or C++ file to create the
    implementation.
*/

#if !defined(SOKOL_GFX_INCLUDED)
#error "Please include sokol_gfx.h before sokol_gfx_utils.h"
#endif

#if defined(SOKOL_API_DECL) && !defined(SOKOL_GFX_UTILS_API_DECL)
#define SOKOL_GFX_UTILS_API_DECL SOKOL_API_DECL
#endif
#ifndef SOKOL_GFX_UTILS_API_DECL
#define SOKOL_GFX_UTILS_API_DECL extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
    sg_readback_request
    Create a request to read back the pixels of an image.
    The image must be a regular image (not a swapchain image).
    Returns a handle to the request, or NULL if it failed.
*/
SOKOL_GFX_UTILS_API_DECL void* sg_readback_request(sg_image img);

/*
    sg_readback_is_ready
    Returns true if the readback request has completed and data is ready to be copied.
*/
SOKOL_GFX_UTILS_API_DECL bool sg_readback_is_ready(void* request);

/*
    sg_readback_wait
    Synchronously wait until the readback request has completed.
*/
SOKOL_GFX_UTILS_API_DECL void sg_readback_wait(void* request);

/*
    sg_readback_copy_data
    Copies the readback data into the provided buffer.
    The buffer must be large enough to hold width * height * 4 bytes (for RGBA8).
    This function should only be called if sg_readback_is_ready returns true.
*/
SOKOL_GFX_UTILS_API_DECL void sg_readback_copy_data(void* request, void* out_pixels);

/*
    sg_readback_destroy
    Frees the resources associated with a readback request.
*/
SOKOL_GFX_UTILS_API_DECL void sg_readback_destroy(void* request);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SOKOL_GFX_UTILS_INCLUDED */

/*-- IMPLEMENTATION ----------------------------------------------------------*/
#ifdef SOKOL_GFX_UTILS_IMPL
#define SOKOL_GFX_UTILS_IMPL_INCLUDED (1)

#include <stdlib.h> // malloc, free
#include <string.h> // memcpy

#ifndef GFX_READBACK_TIMEOUT_MS
#define GFX_READBACK_TIMEOUT_MS (2000)
#endif

#ifndef _SOKOL_PRIVATE
    #if defined(__GNUC__) || defined(__clang__)
        #define _SOKOL_PRIVATE __attribute__((unused)) static
    #else
        #define _SOKOL_PRIVATE static
    #endif
#endif

/*--- Metal Backend ----------------------------------------------------------*/
#if defined(SOKOL_METAL)

#if !defined(__OBJC__)
#error "Metal implementation requires Objective-C"
#endif
#import <Metal/Metal.h>

typedef struct {
    id<MTLBuffer> buffer;
    id<MTLCommandBuffer> cmd_buffer;
    int width;
    int height;
    int padded_bytes_per_row;
    int bytes_per_row;
} _sgu_mtl_readback_request_t;

_SOKOL_PRIVATE void* _sgu_mtl_readback_request(sg_image img) {
    _sg_image_t* img_ptr = _sg_lookup_image(img.id);
    if (!img_ptr || img_ptr->slot.state != SG_RESOURCESTATE_VALID) {
        return NULL;
    }

    int slot = img_ptr->cmn.active_slot;
    int mtl_id = img_ptr->mtl.tex[slot];
    id<MTLTexture> tex = (id<MTLTexture>)_sg.mtl.idpool.pool[(NSUInteger)mtl_id];
    if (!tex) {
        return NULL;
    }

    id<MTLDevice> device = _sg.mtl.device;
    int width = img_ptr->cmn.width;
    int height = img_ptr->cmn.height;
    int bytes_per_pixel = 4; // Assuming RGBA8
    int bytes_per_row = width * bytes_per_pixel;
    int padded_bytes_per_row = (bytes_per_row + 255) & ~255;
    int buffer_size = padded_bytes_per_row * height;

    id<MTLBuffer> read_buffer = [device newBufferWithLength:(NSUInteger)buffer_size options:MTLResourceStorageModeShared];
    id<MTLCommandQueue> queue = _sg.mtl.cmd_queue;
    id<MTLCommandBuffer> cmd_buffer = [queue commandBuffer];
    id<MTLBlitCommandEncoder> blit_encoder = [cmd_buffer blitCommandEncoder];

    [blit_encoder copyFromTexture:tex
                   sourceSlice:0
                   sourceLevel:0
                  sourceOrigin:MTLOriginMake(0, 0, 0)
                    sourceSize:MTLSizeMake((NSUInteger)width, (NSUInteger)height, 1)
                      toBuffer:read_buffer
             destinationOffset:0
        destinationBytesPerRow:(NSUInteger)padded_bytes_per_row
      destinationBytesPerImage:(NSUInteger)buffer_size];

    [blit_encoder endEncoding];
    [cmd_buffer commit];

    _sgu_mtl_readback_request_t* req = (_sgu_mtl_readback_request_t*) malloc(sizeof(_sgu_mtl_readback_request_t));
    req->buffer = read_buffer;
    req->cmd_buffer = cmd_buffer;
#if !__has_feature(objc_arc)
    [req->cmd_buffer retain];
#endif
    req->width = width;
    req->height = height;
    req->padded_bytes_per_row = padded_bytes_per_row;
    req->bytes_per_row = bytes_per_row;

    return (void*)req;
}

_SOKOL_PRIVATE bool _sgu_mtl_readback_is_ready(void* request) {
    _sgu_mtl_readback_request_t* req = (_sgu_mtl_readback_request_t*)request;
    return [req->cmd_buffer status] == MTLCommandBufferStatusCompleted;
}

_SOKOL_PRIVATE void _sgu_mtl_readback_wait(void* request) {
    _sgu_mtl_readback_request_t* req = (_sgu_mtl_readback_request_t*)request;
    if (req->cmd_buffer) {
        int timeout_ms = GFX_READBACK_TIMEOUT_MS;
        while (timeout_ms > 0) {
            MTLCommandBufferStatus status = [req->cmd_buffer status];
            if (status >= MTLCommandBufferStatusCompleted) {
                break;
            }
            [NSThread sleepForTimeInterval:0.001];
            timeout_ms--;
        }
    }
}

_SOKOL_PRIVATE void _sgu_mtl_readback_copy_data(void* request, void* out_pixels) {
    _sgu_mtl_readback_request_t* req = (_sgu_mtl_readback_request_t*)request;
    uint8_t* src = (uint8_t*)[req->buffer contents];
    uint8_t* dst = (uint8_t*)out_pixels;

    if (req->padded_bytes_per_row == req->bytes_per_row) {
        memcpy(dst, src, (size_t)req->width * (size_t)req->height * 4);
    } else {
        for (int i = 0; i < req->height; ++i) {
            memcpy(dst + (size_t)i * (size_t)req->bytes_per_row, src + (size_t)i * (size_t)req->padded_bytes_per_row, (size_t)req->bytes_per_row);
        }
    }
}

_SOKOL_PRIVATE void _sgu_mtl_readback_destroy(void* request) {
    _sgu_mtl_readback_request_t* req = (_sgu_mtl_readback_request_t*)request;
#if !__has_feature(objc_arc)
    [req->buffer release];
    [req->cmd_buffer release];
#endif
    free(req);
}

#endif /* SOKOL_METAL */

/*--- GL Backend -------------------------------------------------------------*/
#if defined(_SOKOL_ANY_GL)

typedef struct {
    GLuint pbo;
    GLsync sync;
    int width;
    int height;
    int bytes_per_row;
} _sgu_gl_readback_request_t;

_SOKOL_PRIVATE void* _sgu_gl_readback_request(sg_image img) {
    _sg_image_t* img_ptr = _sg_lookup_image(img.id);
    if (!img_ptr || img_ptr->slot.state != SG_RESOURCESTATE_VALID) {
        return NULL;
    }

    int slot = img_ptr->cmn.active_slot;
    GLuint tex_id = img_ptr->gl.tex[slot];
    int width = img_ptr->cmn.width;
    int height = img_ptr->cmn.height;
    int bytes_per_row = width * 4;
    int buffer_size = bytes_per_row * height;

    GLuint pbo;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, NULL, GL_STREAM_READ);

    GLuint prev_fbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&prev_fbo);

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id, 0);

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
    glDeleteFramebuffers(1, &fbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    _sgu_gl_readback_request_t* req = (_sgu_gl_readback_request_t*) malloc(sizeof(_sgu_gl_readback_request_t));
    req->pbo = pbo;
    req->sync = sync;
    req->width = width;
    req->height = height;
    req->bytes_per_row = bytes_per_row;
    return (void*)req;
}

_SOKOL_PRIVATE bool _sgu_gl_readback_is_ready(void* request) {
    _sgu_gl_readback_request_t* req = (_sgu_gl_readback_request_t*)request;
    GLenum res = glClientWaitSync(req->sync, 0, 0);
    return (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED);
}

_SOKOL_PRIVATE void _sgu_gl_readback_wait(void* request) {
    _sgu_gl_readback_request_t* req = (_sgu_gl_readback_request_t*)request;
    glClientWaitSync(req->sync, GL_SYNC_FLUSH_COMMANDS_BIT, (GLuint64)GFX_READBACK_TIMEOUT_MS * 1000000);
}

_SOKOL_PRIVATE void _sgu_gl_readback_copy_data(void* request, void* out_pixels) {
    _sgu_gl_readback_request_t* req = (_sgu_gl_readback_request_t*)request;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, req->pbo);
    void* ptr = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, (GLsizeiptr)req->width * (GLsizeiptr)req->height * 4, GL_MAP_READ_BIT);
    if (ptr) {
        memcpy(out_pixels, ptr, (size_t)req->width * (size_t)req->height * 4);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

_SOKOL_PRIVATE void _sgu_gl_readback_destroy(void* request) {
    _sgu_gl_readback_request_t* req = (_sgu_gl_readback_request_t*)request;
    glDeleteBuffers(1, &req->pbo);
    glDeleteSync(req->sync);
    free(req);
}

#endif /* _SOKOL_ANY_GL */

/*--- D3D11 Backend ----------------------------------------------------------*/
#if defined(SOKOL_D3D11)

typedef struct {
    ID3D11Texture2D* staging_tex;
    int width;
    int height;
    int bytes_per_row;
} _sgu_d3d11_readback_request_t;

_SOKOL_PRIVATE void* _sgu_d3d11_readback_request(sg_image img) {
    _sg_image_t* img_ptr = _sg_lookup_image(img.id);
    if (!img_ptr || img_ptr->slot.state != SG_RESOURCESTATE_VALID) {
        return NULL;
    }

    int width = img_ptr->cmn.width;
    int height = img_ptr->cmn.height;
    int bytes_per_row = width * 4;

    ID3D11Device* dev = _sg.d3d11.dev;
    ID3D11DeviceContext* ctx = _sg.d3d11.ctx;
    ID3D11Texture2D* src_tex = img_ptr->d3d11.tex2d;

    D3D11_TEXTURE2D_DESC desc;
    src_tex->GetDesc(&desc);
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.MiscFlags = 0;

    ID3D11Texture2D* staging_tex = NULL;
    HRESULT hr = dev->CreateTexture2D(&desc, NULL, &staging_tex);
    if (FAILED(hr)) {
        return NULL;
    }

    ctx->CopyResource(staging_tex, src_tex);

    _sgu_d3d11_readback_request_t* req = (_sgu_d3d11_readback_request_t*) malloc(sizeof(_sgu_d3d11_readback_request_t));
    req->staging_tex = staging_tex;
    req->width = width;
    req->height = height;
    req->bytes_per_row = bytes_per_row;
    return (void*)req;
}

_SOKOL_PRIVATE bool _sgu_d3d11_readback_is_ready(void* request) {
    _sgu_d3d11_readback_request_t* req = (_sgu_d3d11_readback_request_t*)request;
    ID3D11DeviceContext* ctx = _sg.d3d11.ctx;
    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = ctx->Map(req->staging_tex, 0, D3D11_MAP_READ, D3D11_MAP_FLAG_DO_NOT_WAIT, &mapped);
    if (hr == S_OK) {
        ctx->Unmap(req->staging_tex, 0);
        return true;
    }
    return false;
}

_SOKOL_PRIVATE void _sgu_d3d11_readback_wait(void* request) {
    _sgu_d3d11_readback_request_t* req = (_sgu_d3d11_readback_request_t*)request;
    ID3D11DeviceContext* ctx = _sg.d3d11.ctx;
    int timeout_ms = GFX_READBACK_TIMEOUT_MS;
    while (timeout_ms > 0) {
        D3D11_MAPPED_SUBRESOURCE mapped;
        HRESULT hr = ctx->Map(req->staging_tex, 0, D3D11_MAP_READ, D3D11_MAP_FLAG_DO_NOT_WAIT, &mapped);
        if (hr == S_OK) {
            ctx->Unmap(req->staging_tex, 0);
            break;
        }
        timeout_ms--;
    }
}

_SOKOL_PRIVATE void _sgu_d3d11_readback_copy_data(void* request, void* out_pixels) {
    _sgu_d3d11_readback_request_t* req = (_sgu_d3d11_readback_request_t*)request;
    ID3D11DeviceContext* ctx = _sg.d3d11.ctx;
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(ctx->Map(req->staging_tex, 0, D3D11_MAP_READ, 0, &mapped))) {
        if (mapped.RowPitch == (UINT)req->bytes_per_row) {
            memcpy(out_pixels, mapped.pData, (size_t)req->height * (size_t)req->bytes_per_row);
        } else {
            for (int i = 0; i < req->height; ++i) {
                memcpy((uint8_t*)out_pixels + (size_t)i * (size_t)req->bytes_per_row, (uint8_t*)mapped.pData + (size_t)i * (size_t)mapped.RowPitch, (size_t)req->bytes_per_row);
            }
        }
        ctx->Unmap(req->staging_tex, 0);
    }
}

_SOKOL_PRIVATE void _sgu_d3d11_readback_destroy(void* request) {
    _sgu_d3d11_readback_request_t* req = (_sgu_d3d11_readback_request_t*)request;
    req->staging_tex->Release();
    free(req);
}

#endif /* SOKOL_D3D11 */

/*--- Public API Implementation ----------------------------------------------*/
SOKOL_API_IMPL void* sg_readback_request(sg_image img) {
#if defined(SOKOL_METAL)
    return _sgu_mtl_readback_request(img);
#elif defined(_SOKOL_ANY_GL)
    return _sgu_gl_readback_request(img);
#elif defined(SOKOL_D3D11)
    return _sgu_d3d11_readback_request(img);
#else
    (void)img;
    return NULL;
#endif
}

SOKOL_API_IMPL bool sg_readback_is_ready(void* request) {
    if (!request) return false;
#if defined(SOKOL_METAL)
    return _sgu_mtl_readback_is_ready(request);
#elif defined(_SOKOL_ANY_GL)
    return _sgu_gl_readback_is_ready(request);
#elif defined(SOKOL_D3D11)
    return _sgu_d3d11_readback_is_ready(request);
#else
    return false;
#endif
}

SOKOL_API_IMPL void sg_readback_wait(void* request) {
    if (!request) return;
#if defined(SOKOL_METAL)
    _sgu_mtl_readback_wait(request);
#elif defined(_SOKOL_ANY_GL)
    _sgu_gl_readback_wait(request);
#elif defined(SOKOL_D3D11)
    _sgu_d3d11_readback_wait(request);
#endif
}

SOKOL_API_IMPL void sg_readback_copy_data(void* request, void* out_pixels) {
    if (!request || !out_pixels) return;
#if defined(SOKOL_METAL)
    _sgu_mtl_readback_copy_data(request, out_pixels);
#elif defined(_SOKOL_ANY_GL)
    _sgu_gl_readback_copy_data(request, out_pixels);
#elif defined(SOKOL_D3D11)
    _sgu_d3d11_readback_copy_data(request, out_pixels);
#endif
}

SOKOL_API_IMPL void sg_readback_destroy(void* request) {
    if (!request) return;
#if defined(SOKOL_METAL)
    _sgu_mtl_readback_destroy(request);
#elif defined(_SOKOL_ANY_GL)
    _sgu_gl_readback_destroy(request);
#elif defined(SOKOL_D3D11)
    _sgu_d3d11_readback_destroy(request);
#endif
}

#endif /* SOKOL_GFX_UTILS_IMPL */
