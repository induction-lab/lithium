#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

/* Simple particle system */

class Particle {
public:
    Particle() {
        LOG_DEBUG("Create Particle.");
        // Setup properties of the particle.
        speed = Vector2(frandRange(-X_SPEED_RANGE, X_SPEED_RANGE), frandRange(0.0f, Y_SPEED_RANGE));
        // Set the initial framesToLive count (this counts down to 0 - at 0 the particle is removed).
        framesToLive = FRAMES_TO_LIVE;
        // Randomise the size of the particle.
        size = frandRange(MIN_PARTICLE_SIZE, MAX_PARTICLE_SIZE);
        rotation = frandRange(-1.0f, 1.0f);
     };
    ~Particle() {
        // LOG_DEBUG("Delete Particle.");
    };
    const int FRAMES_TO_LIVE      = 300;
    const float MIN_PARTICLE_SIZE = 0.2f;
    const float MAX_PARTICLE_SIZE = 0.7f;
    const float X_SPEED_RANGE     = 3.0f;
    const float Y_SPEED_RANGE     = 6.0f;
    const float GRAVITY           = 0.1f;
    void update() {
        sprite->angle += rotation;
        sprite->scale = Vector2(size, size);
        // Calculate the new Y speed of the particle.
        speed.y = speed.y - GRAVITY;
        // Update the position of the particle by the speed it's moving at.
        sprite->location = sprite->location + speed * TimeManager::getInstance()->getFrameElapsedTime() * 50.0f;
        // Decrease the frames the particle will live for by 1.
        framesToLive--;
    };
    Vector2 speed;     // the current speed of the particle
    float size;        // the size of the particle
    float rotation;    // the rotation speed
    int framesToLive;  // number of frames before the particle is destroyed
    Sprite* sprite;
};

class ParticleSystem {
public:
    ParticleSystem() {
        LOG_DEBUG("Create ParticleSystem.");
        spriteBatch = new SpriteBatch();
    };
    ~ParticleSystem() {
        LOG_DEBUG("Delete ParticleSystem.");
        LOG_DEBUG("Delete %d particles.", particles.size());
        for (std::vector<Particle*>::iterator it = particles.begin(); it < particles.end(); ++it) {
            SAFE_DELETE(*it);
        }
        particles.clear();
    };
    void update() {
        float renderWidth = (float)GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float)GraphicsManager::getInstance()->getRenderHeight();
        float halfWidth = renderWidth / 2;
        float halfHeight = renderHeight / 2;
        // Iterate over all particles in the vector
        for (std::vector<Particle*>::iterator it = particles.begin(); it < particles.end(); ++it) {
            // If the particle time to live is more than zero...
            if ((*it)->framesToLive > 0) {
                // ...update the particle position and draw it.
                (*it)->update();
            } else { 
                // If it's time to destroy the particle...
                // ...then remove it from the vector...
                spriteBatch->unregisterSprite((*it)->sprite);
                SAFE_DELETE(*it);
                particles.erase(it);
            }
        }
    };
    void addParticle(Vector2 location) {
        // LOG_DEBUG("Create new particle.");
        Particle* particle = new Particle();
        particle->sprite = spriteBatch->registerSprite("textures/Particle.png", 47, 47);
        particle->sprite->location = location;
        particles.push_back(particle);
    };
    void emit(int count, Vector2 location) {
        for (int n = 0; n < count; n++) {
            addParticle(location);
        }
    };
private:
    SpriteBatch* spriteBatch;
    std::vector<Particle*> particles;
};

#endif // __PARTICLESYSTEM_H__