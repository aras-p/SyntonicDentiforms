#include "AnimationBunch.h"
#include "AnimType.h"
#include "SampledAnimation.h"
#include "src/utils/AssertHelper.h"
#include "src/utils/STLUtils.h"


CAnimationBunch::~CAnimationBunch()
{
	stl_utils::wipe_map( mVector3Anims );
	stl_utils::wipe_map( mQuatAnims );
	stl_utils::wipe_map( mFloatAnims );
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
        switch( groupType ) {
        case TYPE_VECTOR3:
            {
                typedef SVector3 TData;
                typedef CSampledAnimation<TData> TAnim;
                typedef CAnimCurve<TData> TCurve;
                assert( sampleSize == sizeof(TData) );
                TAnim* anim = new TAnim( samplesPerCurve, (TAnim::eLoopType)loop );
                // read curves
                anim->reserveCurves( curves );
                for( int c = 0; c < curves; ++c ) {
                    int ipol, firstSample;
                    fread( &ipol, 1, 4, f );
                    fread( &firstSample, 1, 4, f );
                    TData coll;
                    fread( &coll, 1, sizeof(coll), f );
                    anim->addCurve( TCurve( coll, firstSample, (TCurve::eIpol)ipol ) );
                }
                // read samples
                int totalSamples;
                fread( &totalSamples, 1, 4, f );
                anim->resizeSamples( totalSamples );
                if( totalSamples > 0 )
                    fread( (void*)&anim->getSample(0), totalSamples, sizeof(TData), f );
                // add to bunch
                bunch.addVector3Anim( name, *anim );
            }
            break;
        case TYPE_QUATERNION:
            {
                typedef SQuaternion TData;
                typedef CSampledAnimation<TData> TAnim;
                typedef CAnimCurve<TData> TCurve;
                assert( sampleSize == sizeof(TData) );
                TAnim* anim = new TAnim( samplesPerCurve, loop ? (TAnim::REPEAT) : (TAnim::CLAMP) );
                // read curves
                anim->reserveCurves( curves );
                for( int c = 0; c < curves; ++c ) {
                    int ipol, firstSample;
                    fread( &ipol, 1, 4, f );
                    fread( &firstSample, 1, 4, f );
                    TData coll;
                    fread( &coll, 1, sizeof(coll), f );
                    anim->addCurve( TCurve( coll, firstSample, (TCurve::eIpol)ipol ) );
                }
                // read samples
                int totalSamples;
                fread( &totalSamples, 1, 4, f );
                anim->resizeSamples( totalSamples );
                fread( (void*)&anim->getSample(0), totalSamples, sizeof(TData), f );
                // add to bunch
                bunch.addQuatAnim( name, *anim );
            }
            break;
        case TYPE_FLOAT:
            {
                typedef float TData;
                typedef CSampledAnimation<TData> TAnim;
                typedef CAnimCurve<TData> TCurve;
                assert( sampleSize == sizeof(TData) );
                TAnim* anim = new TAnim( samplesPerCurve, loop ? (TAnim::REPEAT) : (TAnim::CLAMP) );
                // read curves
                anim->reserveCurves( curves );
                for( int c = 0; c < curves; ++c ) {
                    int ipol, firstSample;
                    fread( &ipol, 1, 4, f );
                    fread( &firstSample, 1, 4, f );
                    TData coll;
                    fread( &coll, 1, sizeof(coll), f );
                    anim->addCurve( TCurve( coll, firstSample, (TCurve::eIpol)ipol ) );
                }
                // read samples
                int totalSamples;
                fread( &totalSamples, 1, 4, f );
                anim->resizeSamples( totalSamples );
                fread( (void*)&anim->getSample(0), totalSamples, sizeof(TData), f );
                // add to bunch
                bunch.addFloatAnim( name, *anim );
            }
            break;
        default:
            ASSERT_FAIL_MSG( "unknown animation sample type" );
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
        ASSERT_FAIL_MSG("File is not a valid animation file");
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
        {
        ASSERT_FAIL_MSG("Animation file contains unknown anim type");
        }
    }

    // close file
    fclose( f );

    return bunch;
}

