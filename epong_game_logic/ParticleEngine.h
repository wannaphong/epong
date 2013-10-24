/*
 * Copyright (c) 2011 Nokia Corporation.
 */


#ifndef __Particle_ENGINE__
#define __Particle_ENGINE__

class PongApp;
class ParticleEngine;
class Particle;

class ParticleType {
public:
    ParticleType( unsigned int textureID );
    virtual ~ParticleType();

        // Angle and anglevariance
    int m_angle;
    int m_angleRandom;

        // Angle increment/time and radiance for it.
    int m_angleInc;
    int m_angleIncRandom;

        // Size and sizevariance
    int m_size;
    int m_sizeRandom;

        // Size increment/time and vadiance for it
    int m_sizeInc;
    int m_sizeIncRandom;

        // Lifetime in fixed seconds and variance for it.
    int m_lifeTime;
    int m_lifeTimeRandom;

        // Fraction for this type
    int m_fraction;

        // Gravity for this type
    int m_gravity;

        // Angle increments increment/time
    int m_angleIncInc;

        // Size increments increment/time
    int m_sizeIncInc;

        // GLTexture used for rendering this type
    unsigned int textureID;

        // Attributes for this function as real seconds
    void setVisibility( float fadeInTime,
                        float fadeOutTime,
                        float generalVisibility );

    float m_fadeOutTimeSecs;
    float m_fadeInTimeSecs;
    float m_generalVisibility;
    bool m_additiveParticle;

    void setColors( float r, float g, float b,
                    float rr, float gr, float br );
    float col[3];
    float col_random[3];
};



class Particle {
public:
    Particle();
    ~Particle();

    void run( ParticleEngine *engine, int *fixedGravity, int fixedFrameTime );

    int m_pos[3];
    int m_dir[3];
    int m_lifeTime;
    int m_aliveCounter;

    int m_angle;
    int m_angleInc;

    int m_size;
    int m_sizeInc;

    unsigned int color;
    ParticleType *m_type;
};


class ParticleEngine {
public:
    ParticleEngine(int maxParticles );
    virtual ~ParticleEngine();

    void run( float frameTime );
    void setGravity( float *gravityVector ) {
        m_gravity[0] = (int)( gravityVector[0] * 65536.0f );
        m_gravity[1] = (int)( gravityVector[1] * 65536.0f );
        m_gravity[2] = (int)( gravityVector[2] * 65536.0f );
    };

    void render( SpriteBatch *batch, ParticleType *renderType  );

    /*
     * count: how many particles to emit
     * type: pointer to particletype defining the type of particles to emit
     * pos: position of the emit
     * posrandom: variance for position of emit
     * dir: direction of emit
     * dirrandom: variance for direction of emit
     */
    void emitParticles( int count,
                        ParticleType *type,
                        float *pos, float posRandom,
                        float *dir, float dirRandom );



protected:
    int m_gravity[3];
    Particle *m_particles;
    int m_maxParticles;
    int m_currentParticle;

};

#endif
