/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include "PongApp.h"
#include <memory.h>
#include "ParticleEngine.h"
#include <math.h>
#include <stdlib.h>


/**
 * Particle
 */
Particle::Particle() {
    memset( m_pos, 0, sizeof(int)*3);
    memset( m_dir, 0, sizeof(int)*3);
};

Particle::~Particle() {
};

void Particle::run( ParticleEngine *engine, int *fixedGravity, int fixedFrameTime ) {
        // Move
    m_pos[0] += (((m_dir[0]>>2) * fixedFrameTime)>>10);
    m_pos[1] += (((m_dir[1]>>2) * fixedFrameTime)>>10);
    m_pos[2] += (((m_dir[2]>>2) * fixedFrameTime)>>10);

        // Fraction
    int te = ((m_type->m_fraction * fixedFrameTime)>>12);
    if (te>4096) te = 4096;
    m_dir[0] -= (((m_dir[0]>>2) * te ) >> 10);
    m_dir[1] -= (((m_dir[1]>>2) * te ) >> 10);
    m_dir[2] -= (((m_dir[2]>>2) * te ) >> 10);

        // Gravity
    /*
    m_dir[1] -= (((m_type->m_gravity>>2)*fixedFrameTime)>>10);
    */
    m_dir[0] += (((fixedGravity[0]>>2) * (fixedFrameTime>>2))>>12);
    m_dir[1] += (((fixedGravity[1]>>2) * (fixedFrameTime>>2))>>12);
    m_dir[2] += (((fixedGravity[2]>>2) * (fixedFrameTime>>2))>>12);



          // Size increment
    m_size += (((m_sizeInc>>4)*fixedFrameTime)>>8);

        // Angle increment
    m_angle += (((m_angleInc>>4)*fixedFrameTime)>>8);

        // Second phase increments for size and angle increments.
    m_sizeInc += ((m_type->m_sizeIncInc*fixedFrameTime));
    m_angleInc += ((m_type->m_angleIncInc*fixedFrameTime));


    m_lifeTime-=fixedFrameTime;
    m_aliveCounter+=fixedFrameTime;
        // Particle is dead if size has been dropped below one.
    if (m_size<1<<12) m_lifeTime = 0;
};


/*
 * ParticleType
 */
ParticleType::ParticleType(unsigned int tid ) {
    textureID = tid;
    m_angle = 0;
    m_angleRandom = 0;
    m_angleInc = 0;
    m_angleIncRandom = 0;
    m_size = 4096;
    m_sizeRandom = 0;
    m_sizeInc = 0;
    m_sizeIncRandom = 0;
    m_lifeTime = 4096;
    m_lifeTimeRandom = 0;
    m_fraction = 0;
    m_gravity = 0;
    m_angleIncInc = 0;
    m_sizeIncInc = 0;
    m_fadeOutTimeSecs = 1.0f/2000.0f;
    m_additiveParticle = true;
    setVisibility( 0.0f, 0.5f, 1.0f );
    col[0] = 1.0f;
    col[1] = 1.0f;
    col[2] = 1.0f;
    col_random[0] = 0.0f;
    col_random[1] = 0.0f;
    col_random[2] = 0.0f;
};

void ParticleType::setVisibility( float fadeInTime,
                                  float fadeOutTime,
                                  float generalVisibility ) {
    m_fadeOutTimeSecs = 1.0f / (fadeOutTime);
    m_fadeInTimeSecs = 1.0f / (fadeInTime);
    m_generalVisibility = generalVisibility;
};

void ParticleType::setColors( float r, float g, float b,
                               float rr, float gr, float br ) {
    col[0] = r;
    col[1] = g;
    col[2] = b;
    col_random[0] = rr;
    col_random[1] = gr;
    col_random[2] = br;
};

ParticleType::~ParticleType() {
};



/*
 * ParticleEngine
 */
ParticleEngine::ParticleEngine( int maxParticles ) {
    m_particles = new Particle[ maxParticles ];
    memset( m_particles, 0, sizeof( Particle ) * maxParticles );
    m_maxParticles = maxParticles;
    m_currentParticle = 0;

};



ParticleEngine::~ParticleEngine() {
    delete [] m_particles;
};

void ParticleEngine::render( SpriteBatch *batch, ParticleType *renderType ) {

    if (renderType==0) return;

    SpriteDrawInfo sdi;
    float ftemp;
    Particle *p = m_particles;
    Particle *p_target = m_particles + m_maxParticles;
#ifdef EPONG_DEBUG
    while (p!=p_target) {
        if (p->m_type==renderType) {
            sdi.setTargetPos( (float)m_gravity[0]/40000.0f, (float)m_gravity[1]/80000.0f);
            sdi.textureHandle = p->m_type->textureID;
            sdi.setScale( 0.25f );
            sdi.setColor( 255.0f, 255.0f, 255.0f, 0.5f);
            sdi.a = 0.5f;
            batch->draw( &sdi);
            break;
        }
        p++;
    }
    p = m_particles;
#endif

    while (p!=p_target) {
        if (p->m_type==renderType && p->m_lifeTime>0) {
            sdi.setTargetPos( (float)p->m_pos[0]/4096.0f, (float)p->m_pos[1]/4096.0f );
            sdi.textureHandle = p->m_type->textureID;
            sdi.setScale( (float)p->m_size * 2.0f / 409600.0f, (float)p->m_size * 2.0f / 409600.0f );
            sdi.setColor( (float)((p->color>>0)&255)/255.0f, (float)((p->color>>8)&255)/255.0f, (float)((p->color>>16)&255)/255.0f, 1.0f);

            sdi.a =  p->m_type->m_generalVisibility*(float)p->m_lifeTime/(65536.0f/4.0f)*p->m_type->m_fadeOutTimeSecs;
            ftemp = ((float)p->m_aliveCounter / (65536.0f/4.0f)) * p->m_type->m_fadeInTimeSecs;
            if (sdi.a == p->m_type->m_generalVisibility)
                sdi.a = p->m_type->m_generalVisibility;
            if (ftemp<sdi.a) sdi.a = ftemp;

            batch->draw( &sdi );

        };
        p++;
    };
}


void ParticleEngine::run( float frameTime ) {
        // -> 12bit fixedpoint
    int fixedFrameTime = (int)( frameTime * 4096.0f );

    Particle *p = m_particles;
    Particle *p_target = p+m_maxParticles;
    while (p!=p_target) {
        if (p->m_lifeTime>0) p->run( this, m_gravity, fixedFrameTime );
        p++;
    };
}


void ParticleEngine::emitParticles( int count,
                                    ParticleType *type,
                                    float *pos,
                                    float posRandom,
                                    float *dir,
                                    float dirRandom ) {

    int fixed_p[3];
    int fixed_d[3];

    int fixedPosRandom = (int)( posRandom*256.0f );
    int fixedDirRandom = (int)( dirRandom*256.0f );

    fixed_p[0] = (int)(pos[0]*4096.0f);
    fixed_p[1] = (int)(pos[1]*4096.0f);
    fixed_p[2] = (int)(pos[2]*4096.0f);

    if (dir) {
        fixed_d[0] = (int)(dir[0]*4096.0f);
        fixed_d[1] = (int)(dir[1]*4096.0f);
        fixed_d[2] = (int)(dir[2]*4096.0f);
    } else memset( fixed_d, 0, sizeof( int ) *3 );

    int fixed_r[3];
    int temp;
    float c[3];

    Particle *p;
    while (count>0) {
        p = m_particles + m_currentParticle;
        p->m_type = type;

        p->m_aliveCounter = 0;

                        // create a random vector
        fixed_r[0] = (rand()&255)-128;
        fixed_r[1] = (rand()&255)-128;
        fixed_r[2] = (rand()&255)-128;
        temp = (int)sqrtf( fixed_r[0] * fixed_r[0]
                          + fixed_r[1]*fixed_r[1]
                          + fixed_r[2]*fixed_r[2] );
        if (temp>0) {
            fixed_r[0] = (fixed_r[0]<<16)/temp;
            fixed_r[1] = (fixed_r[1]<<16)/temp;
            fixed_r[2] = (fixed_r[2]<<16)/temp;
        }
            // random vector is

                    // position
        p->m_pos[0] = ((fixed_r[0]*fixedPosRandom)>>12) + fixed_p[0];
        p->m_pos[1] = ((fixed_r[1]*fixedPosRandom)>>12) + fixed_p[1];
        p->m_pos[2] = ((fixed_r[2]*fixedPosRandom)>>12) + fixed_p[2];
            // direction
        p->m_dir[0] = ((fixed_r[0]*fixedDirRandom)>>12) + fixed_d[0];
        p->m_dir[1] = ((fixed_r[1]*fixedDirRandom)>>12) + fixed_d[1];
        p->m_dir[2] = ((fixed_r[2]*fixedDirRandom)>>12) + fixed_d[2];

        //p->m_dir[0] = 800000;
        //p->m_dir[1] = 0;
        //p->m_dir[2] = 0;

        p->m_angle = type->m_angle
                + (((rand()&255)*type->m_angleRandom) >> 8);
        p->m_angleInc = type->m_angleInc
                + (((rand()&255)*type->m_angleIncRandom) >> 8);

        p->m_size = type->m_size
                + (((rand()&255)*type->m_sizeRandom) >> 8);
        p->m_sizeInc = type->m_sizeInc
                + (((rand()&255)*type->m_sizeIncRandom) >> 8);

        p->m_lifeTime = type->m_lifeTime
                + (((rand()&255)*type->m_lifeTimeRandom) >> 8);


        // colro
        c[0] = type->col[0]+((float)(rand()&255)/255.0f)*type->col_random[0];
        c[1] = type->col[1]+((float)(rand()&255)/255.0f)*type->col_random[1];
        c[2] = type->col[2]+((float)(rand()&255)/255.0f)*type->col_random[2];

        if (c[0]>1.0f) c[0] = 1.0f; if (c[0]<0.0f) c[0] = 0.0f;
        if (c[1]>1.0f) c[1] = 1.0f; if (c[1]<0.0f) c[1] = 0.0f;
        if (c[2]>1.0f) c[2] = 1.0f; if (c[2]<0.0f) c[2] = 0.0f;

        p->color = (unsigned int)(c[0]*255.0f)
                | ((unsigned int)(c[1]*255.0f)<<8)
                | ((unsigned int)(c[2]*255.0f)<<16);

        count--;
        m_currentParticle++;
        if (m_currentParticle>=m_maxParticles) m_currentParticle = 0;
    };
}

