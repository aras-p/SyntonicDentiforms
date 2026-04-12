#include "AnimationBunch.h"
#include "AnimType.h"
#include "SampledAnimation.h"
#include <assert.h>


CAnimationBunch::~CAnimationBunch()
{
}

static inline std::string gReadString( FILE* f )
{
    std::string str;
    while( true ) {
        int c = fgetc( f );
        if( c==0 || c==EOF )
            break;
        str += (char)c;
    }
    return str;
}

static void readSampledData(CAnimationBunch& bunch, FILE* f, int loop, int curves, int groups)
{
    // samples per curve
    int samplesPerCurve;
    fread( &samplesPerCurve, 1, 4, f );
    assert( samplesPerCurve > 0 );

    for( int g = 0; g < groups; ++g ) {
        // group type
        int groupType;
        fread( &groupType, 1, 4, f );
        // sample size
        int sampleSize;
        fread( &sampleSize, 1, 4, f );
        // group name
        std::string name = gReadString( f );

        SAnimCurve curve;
        static_assert(sizeof(curve.ipol) == 4, "Expected ipol to be 4 bytes");
        switch( groupType ) {
        case TYPE_VECTOR3:
            {
                typedef SVector3 TData;
                typedef CSampledAnimation<TData> TAnim;
                assert( sampleSize == sizeof(TData) );
                TAnim &anim = bunch.addVector3Anim();
                anim.init(name, samplesPerCurve, (TAnim::eLoopType)loop);
                // read curves
                anim.reserveCurves( curves );
                for( int c = 0; c < curves; ++c ) {
                    fread( &curve.ipol, 1, 4, f );
                    fread( &curve.firstSampleIndex, 1, 4, f );
                    fread( &curve.collapsedValue, 1, sampleSize, f );
                    anim.addCurve(curve);
                }
                // read samples
                int totalSamples;
                fread( &totalSamples, 1, 4, f );
                anim.resizeSamples( totalSamples );
                if( totalSamples > 0 )
                    fread( (void*)&anim.getSample(0), totalSamples, sizeof(TData), f );
            }
            break;
        case TYPE_QUATERNION:
            {
                typedef SQuaternion TData;
                typedef CSampledAnimation<TData> TAnim;
                assert( sampleSize == sizeof(TData) );
                TAnim& anim = bunch.addQuatAnim();
                anim.init(name, samplesPerCurve, loop ? (TAnim::REPEAT) : (TAnim::CLAMP));
                // read curves
                anim.reserveCurves( curves );
                for( int c = 0; c < curves; ++c ) {
                    fread(&curve.ipol, 1, 4, f);
                    fread(&curve.firstSampleIndex, 1, 4, f);
                    fread(&curve.collapsedValue, 1, sampleSize, f);
                    anim.addCurve(curve);
                }
                // read samples
                int totalSamples;
                fread( &totalSamples, 1, 4, f );
                anim.resizeSamples( totalSamples );
                fread( (void*)&anim.getSample(0), totalSamples, sizeof(TData), f );
            }
            break;
        case TYPE_FLOAT:
            {
                typedef float TData;
                typedef CSampledAnimation<TData> TAnim;
                assert( sampleSize == sizeof(TData) );
                TAnim& anim = bunch.addFloatAnim();
                anim.init(name, samplesPerCurve, loop ? (TAnim::REPEAT) : (TAnim::CLAMP));
                // read curves
                anim.reserveCurves( curves );
                for( int c = 0; c < curves; ++c ) {
                    fread(&curve.ipol, 1, 4, f);
                    fread(&curve.firstSampleIndex, 1, 4, f);
                    fread(&curve.collapsedValue, 1, sampleSize, f);
                    anim.addCurve(curve);
                }
                // read samples
                int totalSamples;
                fread( &totalSamples, 1, 4, f );
                anim.resizeSamples( totalSamples );
                fread( (void*)&anim.getSample(0), totalSamples, sizeof(TData), f );
            }
            break;
        default:
            assert(false);
        }
    }
}


CAnimationBunch* load_animation(const char* path)
{
    // try open file
    FILE* f = fopen(path, "rb");
    if( !f )
        return NULL;

    // create bunch
    CAnimationBunch* bunch = new CAnimationBunch();
    assert( bunch );

    // read magic
    char magic[4];
    fread( &magic, 1, 4, f );
    if( magic[0]!='D' || magic[1]!='A' || magic[2]!='N' || magic[3]!='I' )
    {
        assert(false);
        return nullptr;
    }
    // read anim type
    int animType;
    fread( &animType, 1, 4, f );
    // loop
    int loopType;
    fread( &loopType, 1, 4, f );
    // curve count
    int curveCount;
    fread( &curveCount, 1, 4, f );
    // group count
    int groupCount;
    fread( &groupCount, 1, 4, f );

    // read curve infos
    for( int i = 0; i < curveCount; ++i ) {
        std::string name = gReadString( f );
        int parent; // unused
        fread( &parent, 1, 4, f );
        bunch->addCurveDesc(name);
    }
    
    // read rest based on anim type
    switch( animType ) {
    case 0: // sampled
        readSampledData( *bunch, f, loopType, curveCount, groupCount );
        break;
    default:
        assert(false);
    }

    // close file
    fclose( f );

    return bunch;
}

