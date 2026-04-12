#include "Animation.h"


void CSampledAnimation::sample(float time, int firstCurve, int numCurves, float* dest) const
{
    assert(firstCurve >= 0 && firstCurve < getCurveCount());
    assert(numCurves > 0 && numCurves <= getCurveCount());
    assert(firstCurve + numCurves <= getCurveCount());
    assert(dest);

    // get sample indices and alpha
    int sampleIdx1, sampleIdx2;
    float alpha;
    timeToIndex(time, sampleIdx1, sampleIdx2, alpha);

    const int sampleSizeFloats = getFloatsPerSample();
    const int sampleSizeBytes = sampleSizeFloats * sizeof(float);

    for (int i = 0; i < numCurves; ++i)
    {
        const SAnimCurve& curve = getCurve(firstCurve + i);
        switch (curve.ipol) {
        case SAnimCurve::NONE:
            memcpy(dest, &curve.collapsedValue, sampleSizeBytes);
            break;
        case SAnimCurve::STEP:
            memcpy(dest, &mSampleData[(curve.firstSampleIndex + sampleIdx1) * sampleSizeFloats], sampleSizeBytes);
            break;
        case SAnimCurve::LINEAR:
            const float* s1 = &mSampleData[(curve.firstSampleIndex + sampleIdx1) * sampleSizeFloats];
            const float* s2 = &mSampleData[(curve.firstSampleIndex + sampleIdx2) * sampleSizeFloats];
            switch (mSampleType) {
            case TYPE_FLOAT:
                *dest = *s1 + (*s2 - *s1) * alpha;
                break;
            case TYPE_VECTOR3:
                *(SVector3*)dest = *(const SVector3*)s1 + (*(const SVector3*)s2 - *(const SVector3*)s1) * alpha;
                break;
            case TYPE_QUATERNION:
                ((SQuaternion*)dest)->slerp(*(const SQuaternion*)s1, *(const SQuaternion*)s2, alpha);
                break;
            }
        };
        dest += sampleSizeFloats;
    };
}


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

        CSampledAnimation& anim = bunch.addAnim();
        anim.init((eAnimType)groupType, name, samplesPerCurve, (CSampledAnimation::eLoopType)loop);
        // read curves
        anim.reserveCurves(curves);
        for (int c = 0; c < curves; ++c) {
            fread(&curve.ipol, 1, 4, f);
            fread(&curve.firstSampleIndex, 1, 4, f);
            fread(&curve.collapsedValue, 1, sampleSize, f);
            anim.addCurve(curve);
        }
        // read samples
        int totalSamples;
        fread(&totalSamples, 1, 4, f);
        anim.resizeSamples(totalSamples);
        if (totalSamples > 0)
            fread(anim.getSampleData(), sampleSize, totalSamples, f);
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

