/*
Author: Henrique Neffa
Course: CS106B - Programming Abstractions
Assignment: Assignment 6 - Linked Lists
Instructor: Christopher Gregg
*/

#include "ParticleSystem.h"

// Included according to instructions
#include "DrawParticle.h"

using namespace std;


// Constructor -- set starting point for pointers and for numParticles
ParticleSystem::ParticleSystem() {
    _head = nullptr;
    _tail = nullptr;
    _numParticles = 0;
}

// Destructor -- deallocate memory
// Assigns head to 2nd element and delete former head element, until we run out of list
ParticleSystem::~ParticleSystem() {
    while (_head != nullptr) {
        ParticleCell* next = _head->next;
        delete _head;
        _head=next;
    }
}

// Just return number of particles given counting is taken place at add
int ParticleSystem::numParticles() const {
    return _numParticles;
}

//Adds new particle to the scene taking a particle as input (adds to the tail of linked list)
void ParticleSystem::add(Particle particle) {
    // Milestone 4
    // Check before adding if particle is in bounds and has non-negative lifetime
    if (particle.x < 0 || particle.y < 0 || particle.x >= SCENE_WIDTH || particle.y >= SCENE_HEIGHT || particle.lifetime < 0) {
        return;
    }

    // Create new Particle pointer w/ Particle input as characteristics
    ParticleCell* addedParticle = new ParticleCell;
    addedParticle->particle = particle;
    addedParticle->next = nullptr;
    // Case of empty list...
    if (_head == nullptr) {
        _head  = addedParticle;
        addedParticle->prev = nullptr;
    // ...non-empty adding
    } else {
        addedParticle->prev = _tail;
        _tail->next = addedParticle;
    }

    _tail=addedParticle;
    _numParticles++;
}

// Draw particles from head of list to tail
void ParticleSystem::drawParticles() const {
    ParticleCell* curr = _head;
    while (curr != nullptr) {
        drawParticle(curr->particle.x, curr->particle.y, curr->particle.color);
        curr = curr->next;
    }
}

// Moves particles
// Iterates through all our list from head to tail adding dx/dy into x/y
void ParticleSystem::moveParticles() {
    ParticleCell* current = _head;
    ParticleCell* next;

    while (current != nullptr) {
        // Particle movement based on type
        switch (current->particle.type) {
        case ParticleType::STREAMER:
            current->particle.x += current->particle.dx;
            current->particle.y += current->particle.dy;
            break;
        case ParticleType::BALLISTIC:
            current->particle.x += current->particle.dx;
            current->particle.y += current->particle.dy;
            // Ballistic acceleration pattern
            current->particle.dy++;
            break;
        case ParticleType::FIREWORK:
            current->particle.x += current->particle.dx;
            current->particle.y += current->particle.dy;
            // Same as Ballistic
            current->particle.dy++;
            break;
        }

        // decrease lifetime
        current->particle.lifetime--;

        // Check if not in-bounds and lifetime
        if (current->particle.x < 0 || current->particle.y < 0 || current->particle.x >= SCENE_WIDTH || current->particle.y >= SCENE_HEIGHT || current->particle.lifetime < 0) {
            // Particle is out of bounds or its lifetime has ended, remove it
            // ...store next element
            next = current->next;
            // If middle or tail of list, reallocate next element of previous. Else design next as head
            if (current->prev != nullptr) {
                current->prev->next = current->next;
            } else {
                _head=current->next;
            }
            // Assign new previous element to next element. If next is nullptr, assign previous as tail;
            if (current->next != nullptr) {
                current->next->prev = current->prev;
            } else {
                _tail=current->prev;
            }

            //generate streamers for firewrok
            if (current->particle.type == ParticleType::FIREWORK && current->particle.lifetime < 0) {
                Color fireworkColor = Color::random();
                for (int i = 0; i < 50; ++i) {
                    Particle streamer;
                    streamer.x = current->particle.x;
                    streamer.y = current->particle.y;
                    streamer.dx = randomReal(-3, 3);
                    streamer.dy = randomReal(-3, 3);
                    streamer.lifetime = randomInteger(2, 10);
                    streamer.color = fireworkColor;
                    streamer.type = ParticleType::STREAMER;
                    // Add particle
                    add(streamer);
                }
            }

            // Free up memory of deleted
            delete current;
            current = next;
            _numParticles--;
        } else {
            // Particle is still in bounds and has a positive lifetime, keep it.
            current = current->next;
        }
    }
}

/* * * * * Test Cases Below This Point * * * * */

// Milestone 1
STUDENT_TEST("Test add & numParticles") {
    ParticleSystem system;

    EXPECT_EQUAL(system.numParticles(), 0);

    Particle p1, p2, p3;
    system.add(p1);
    EXPECT_EQUAL(system.numParticles(), 1);
    system.add(p2);
    EXPECT_EQUAL(system.numParticles(), 2);
    system.add(p3);
    EXPECT_EQUAL(system.numParticles(), 3);
}



/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Milestone 1: Constructor creates an empty particle system.") {
    ParticleSystem system;
    EXPECT_EQUAL(system._head, nullptr);
}

PROVIDED_TEST("Milestone 1: Empty system has no particles.") {
    ParticleSystem system;
    EXPECT_EQUAL(system.numParticles(), 0);
}

PROVIDED_TEST("Milestone 1: Can add a single particle.") {
    ParticleSystem system;

    Particle particle;
    particle.x = 13.7;
    particle.y = 4.2;
    particle.color = Color::CYAN;

    system.add(particle);

    /* Should have one particle. */
    EXPECT_EQUAL(system.numParticles(), 1);

    /* Invasively check to make sure the _head pointer isn't null,
     * since it needs to hold our particle.
     */
    EXPECT_NOT_EQUAL(system._head, nullptr);

    /* Make sure the particle's x, y, and color are copied over. */
    EXPECT_EQUAL(system._head->particle.x, particle.x);
    EXPECT_EQUAL(system._head->particle.y, particle.y);
    EXPECT_EQUAL(system._head->particle.color, particle.color);

    /* Make sure the list is wired correctly. */
    EXPECT_EQUAL(system._head->prev, nullptr);
    EXPECT_EQUAL(system._head->next, nullptr);
}

PROVIDED_TEST("Milestone 1: Can add two particles.") {
    ParticleSystem system;

    /* Make two particles. Use the x and y coordinates to tell them apart. */
    Particle one, two;
    one.x = 1;
    two.x = 2;

    /* Add each to the particle system. The order should be one, then two. */
    system.add(one);
    system.add(two);

    /* Make sure we see two particles. */
    EXPECT_EQUAL(system.numParticles(), 2);

    /* Make sure the list has two items in it. */
    EXPECT_NOT_EQUAL(system._head, nullptr);
    EXPECT_NOT_EQUAL(system._head->next, nullptr);
    EXPECT_EQUAL(system._head->next->next, nullptr);

    /* Make sure previous pointers work correctly. */
    EXPECT_EQUAL(system._head->prev, nullptr);
    EXPECT_EQUAL(system._head->next->prev, system._head);

    /* Make sure the particles are in the right order. */
    EXPECT_EQUAL(system._head->particle.x, 1);
    EXPECT_EQUAL(system._head->next->particle.x, 2);
}

PROVIDED_TEST("Milestone 1: Can add multiple particles.") {
    ParticleSystem system;

    /* Add some particles. */
    const int kNumParticles = 5;
    for (int i = 0; i < kNumParticles; i++) {
        Particle particle;
        particle.x = i;
        system.add(particle);
    }

    EXPECT_EQUAL(system.numParticles(), kNumParticles);

    /* Confirm they're there and in the right order. */
    int numSeen = 0;
    ParticleSystem::ParticleCell* prev = nullptr;
    ParticleSystem::ParticleCell* curr = system._head;

    /* Walk the list checking particles. */
    while (curr != nullptr) {
        /* x coordinate tracks which particle this is, so this is a way of
         * checking whether we've got the particles in the right order.
         */
        EXPECT_EQUAL(curr->particle.x, numSeen);

        /* Check the list wiring - we should point back to the cell before us,
         * or to nullptr if there are no cells here.
         */
        EXPECT_EQUAL(curr->prev, prev);

        /* Advance forward. */
        prev = curr;
        curr = curr->next;
        numSeen++;
    }

    /* Confirm we saw the right number. */
    EXPECT_EQUAL(numSeen, kNumParticles);
    EXPECT_EQUAL(system.numParticles(), kNumParticles);
}

PROVIDED_TEST("Milestone 1: Stress Test: Efficiently adds and counts particles.") {
    ParticleSystem system;

    /* Add a bunch of particles. If the add operation takes time O(n), this
     * will run very, very slowly. If the add operation takes time O(1),
     * this will run very quickly.
     */
    const int kNumParticles = 100000;
    EXPECT_COMPLETES_IN(5.0, {
        for (int i = 0; i < kNumParticles; i++) {
            Particle particle;
            particle.x = i / (kNumParticles * 1.0);
            system.add(particle);
        }
    });

    /* Count how many particles there are. If this takes time O(n), this
     * will run very, very slowly. If the numParticles operation takes
     * time O(1), this will run almost instantaneously.
     */
    EXPECT_COMPLETES_IN(1.0, {
        const int kNumTimes = 100000;
        for (int i = 0; i < kNumTimes; i++) {
            EXPECT_EQUAL(system.numParticles(), kNumParticles);
        }
    });
}

#include "Demos/ParticleCatcher.h"

PROVIDED_TEST("Milestone 2: Single particle gets drawn.") {
    ParticleSystem system;

    /* This ParticleCatcher will write down all the calls to the global
     * drawParticle function so that we can see what's drawn.
     */
    ParticleCatcher catcher;

    /* Draw all the particles. This should do nothing. */
    system.drawParticles();

    /* Make sure nothing was drawn. */
    EXPECT_EQUAL(catcher.numDrawn(), 0);

    /* Now add a particle. */
    Particle particle;
    particle.x = 13.7;
    particle.y = 4.2;
    particle.color = Color::random();
    system.add(particle);

    /* Draw everything again. We should see this particle. */
    system.drawParticles();
    EXPECT_EQUAL(catcher.numDrawn(), 1);
    EXPECT_EQUAL(catcher[0].x,     particle.x);
    EXPECT_EQUAL(catcher[0].y,     particle.y);
    EXPECT_EQUAL(catcher[0].color, particle.color);
}

PROVIDED_TEST("Milestone 2: Multiple particles are drawn in order.") {
    ParticleSystem system;

    /* Add three particles in a specific order. */
    Particle one, two, three;
    one.color   = Color::CYAN;
    two.color   = Color::YELLOW;
    three.color = Color::MAGENTA;

    system.add(one);
    system.add(two);
    system.add(three);

    /* Draw the particles and confirm they come back in the right order. */
    ParticleCatcher catcher;
    system.drawParticles();

    EXPECT_EQUAL(catcher.numDrawn(), 3);
    EXPECT_EQUAL(catcher[0].color, Color::CYAN);
    EXPECT_EQUAL(catcher[1].color, Color::YELLOW);
    EXPECT_EQUAL(catcher[2].color, Color::MAGENTA);
}

PROVIDED_TEST("Milestone 3: Can move a single particle.") {
    ParticleSystem system;

    /* The particle we'll move around. */
    Particle particle;
    particle.x = 100;
    particle.y = 100;
    particle.dx = 3;
    particle.dy = -4;
    system.add(particle);
    EXPECT_EQUAL(system.numParticles(), 1);

    /* Confirm we have all the right information stored. */
    EXPECT_NOT_EQUAL(system._head, nullptr);
    EXPECT_EQUAL(system._head->particle.x,     particle.x);
    EXPECT_EQUAL(system._head->particle.y,     particle.y);
    EXPECT_EQUAL(system._head->particle.color, particle.color);
    EXPECT_EQUAL(system._head->particle.dx,    particle.dx);
    EXPECT_EQUAL(system._head->particle.dy,    particle.dy);

    /* Move the particle. */
    system.moveParticles();

    /* The particle should be in a new spot with the same initial velocity. */
    EXPECT_EQUAL(system._head->particle.x,     particle.x + particle.dx);
    EXPECT_EQUAL(system._head->particle.y,     particle.y + particle.dy);
    EXPECT_EQUAL(system._head->particle.color, particle.color);
    EXPECT_EQUAL(system._head->particle.dx,    particle.dx);
    EXPECT_EQUAL(system._head->particle.dy,    particle.dy);

    /* Move the particle again. */
    system.moveParticles();

    /* The particle should be in a new spot with the same initial velocity. */
    EXPECT_EQUAL(system._head->particle.x,     particle.x + 2 * particle.dx);
    EXPECT_EQUAL(system._head->particle.y,     particle.y + 2 * particle.dy);
    EXPECT_EQUAL(system._head->particle.color, particle.color);
    EXPECT_EQUAL(system._head->particle.dx,    particle.dx);
    EXPECT_EQUAL(system._head->particle.dy,    particle.dy);
}

PROVIDED_TEST("Milestone 3: Can move multiple particles with different velocities.") {
    ParticleSystem system;

    /* Set up the particles. */
    Particle left, down, diag;
    left.dx = 1;
    left.dy = 0;

    down.dx = 0;
    down.dy = 1;

    diag.dx = 1;
    diag.dy = 1;

    system.add(left);
    system.add(down);
    system.add(diag);

    /* Move the particles multiple times and confirm they're in the right
     * places.
     */
    const int kNumSteps = 50;
    for (int i = 0; i < kNumSteps; i++) {
        /* Draw the particles to see where they are. */
        ParticleCatcher catcher;
        system.drawParticles();

        /* Make sure we see all three particles and that they're in the right
         * places.
         */
        EXPECT_EQUAL(catcher.numDrawn(), 3);

        /* Left */
        EXPECT_EQUAL(catcher[0].x, i);
        EXPECT_EQUAL(catcher[0].y, 0);

        /* Down */
        EXPECT_EQUAL(catcher[1].x, 0);
        EXPECT_EQUAL(catcher[1].y, i);

        /* Diag */
        EXPECT_EQUAL(catcher[2].x, i);
        EXPECT_EQUAL(catcher[2].y, i);

        /* Move the particles for the next iteration. */
        system.moveParticles();
    }
}

PROVIDED_TEST("Milestone 4: Don't add invalid particles to the list.") {
    ParticleSystem system;

    /* Add an unobjectionable particle. */
    Particle good;
    good.x = 3;
    good.y = 5;
    good.color = Color::BlUE;
    good.lifetime = 137;

    /* Add the particle; confirm it's there. */
    system.add(good);
    EXPECT_NOT_EQUAL(system._head, nullptr);
    EXPECT_EQUAL(system._head->particle.x,        good.x);
    EXPECT_EQUAL(system._head->particle.y,        good.y);
    EXPECT_EQUAL(system._head->particle.color,    good.color);
    EXPECT_EQUAL(system._head->particle.lifetime, good.lifetime);
    EXPECT_EQUAL(system.numParticles(), 1);

    /* Now make a mix of bad particles that are out of bounds. */
    Particle bad;

    bad = good;
    bad.x = -0.001; // x too low
    system.add(bad);
    EXPECT_EQUAL(system.numParticles(), 1);

    bad = good;
    bad.y = -0.001; // y too low
    system.add(bad);
    EXPECT_EQUAL(system.numParticles(), 1);

    bad = good;
    bad.x = SCENE_WIDTH; // x too high
    system.add(bad);
    EXPECT_EQUAL(system.numParticles(), 1);

    bad = good;
    bad.y = SCENE_HEIGHT; // y too high
    system.add(bad);
    EXPECT_EQUAL(system.numParticles(), 1);

    bad = good;
    bad.lifetime = -1; // Negative lifetime
    system.add(bad);
    EXPECT_EQUAL(system.numParticles(), 1);

    /* Make sure the first particle is still there and unchanged. */
    EXPECT_EQUAL(system._head->particle.x,        good.x);
    EXPECT_EQUAL(system._head->particle.y,        good.y);
    EXPECT_EQUAL(system._head->particle.color,    good.color);
    EXPECT_EQUAL(system._head->particle.lifetime, good.lifetime);

    /* Make sure the list is still valid. */
    EXPECT_EQUAL(system._head->next, nullptr);
    EXPECT_EQUAL(system._head->prev, nullptr);
}

PROVIDED_TEST("Milestone 4: Particle removed when it leaves the screen.") {
    ParticleSystem system;

    /* A particle moving out of the world. */
    Particle escapee;
    escapee.x = 1.5;
    escapee.y = 1.5;
    escapee.dx = -1;
    escapee.dy = -1;
    escapee.lifetime = 1000;
    system.add(escapee);

    /* Use a ParticleCatcher to see which particles are drawn. */
    ParticleCatcher catcher;

    /* Initially, we should find our particle where we created it. */
    system.drawParticles();

    EXPECT_EQUAL(catcher.numDrawn(), 1);
    EXPECT_EQUAL(catcher[0].x, 1.5);
    EXPECT_EQUAL(catcher[0].y, 1.5);
    catcher.reset(); // Forget we saw the particle

    /* Moving the particle one step should leave the particle alive. */
    system.moveParticles();
    system.drawParticles();

    EXPECT_EQUAL(catcher.numDrawn(), 1);
    EXPECT_EQUAL(catcher[0].x, 0.5);
    EXPECT_EQUAL(catcher[0].y, 0.5);
    catcher.reset(); // Forget we saw the particle

    /* Moving the particle once more should remove the particle because
     * it'll be at position (-0.5, -0.5).
     */
    system.moveParticles();
    system.drawParticles();

    EXPECT_EQUAL(catcher.numDrawn(), 0);
    EXPECT_EQUAL(system.numParticles(), 0);
}

PROVIDED_TEST("Milestone 4: Particle removed when its lifetime ends.") {
    ParticleSystem system;

    /* A particle moving out of the world. */
    Particle timeout;
    timeout.x = 1.5;
    timeout.y = 1.5;
    timeout.dx = 1;
    timeout.dy = 1;
    timeout.lifetime = 1;
    system.add(timeout);

    /* Use a ParticleCatcher to see which particles are drawn. */
    ParticleCatcher catcher;

    /* Initially, we should find our particle where we created it. */
    system.drawParticles();

    EXPECT_EQUAL(catcher.numDrawn(), 1);
    EXPECT_EQUAL(catcher[0].x, 1.5);
    EXPECT_EQUAL(catcher[0].y, 1.5);
    catcher.reset(); // Forget we saw the particle

    /* Moving the particle one step should leave the particle alive but with
     * a lifetime of 0.
     */
    system.moveParticles();
    system.drawParticles();

    EXPECT_EQUAL(catcher.numDrawn(), 1);
    EXPECT_EQUAL(catcher[0].x, 2.5);
    EXPECT_EQUAL(catcher[0].y, 2.5);
    catcher.reset(); // Forget we saw the particle

    /* Moving the particle once more should remove the particle because its
     * lifetime becomes negative.
     */
    system.moveParticles();
    system.drawParticles();

    EXPECT_EQUAL(catcher.numDrawn(), 0);
    EXPECT_EQUAL(system.numParticles(), 0);
}

PROVIDED_TEST("Milestone 4: All particles move even if first needs to be removed.") {
    /* Create a list of five particles at positions (0, 0), (1, 0), ... (4, 0).
     * All particles move downward. All particles have a long lifetime except
     * for the first, which has a lifetime of 0 and thus disappears as soon as
     * it moves. We should see all the others shifted down by one spot.
     */
    ParticleSystem system;

    /* Used to identify particles. */
    const Vector<Color> colors = {
        Color::BLACK, Color::WHITE, Color::RED, Color::GREEN, Color::YELLOW
    };

    /* Initialize the particles. */
    Vector<Particle> particles;
    for (int i = 0; i < 5; i++) {
        Particle particle;
        particle.x = i;
        particle.dx = 0;
        particle.dy = 1;
        particle.lifetime = 100;
        particle.color = colors[i]; // So we can flag it later
        particles += particle;
    }
    particles[0].lifetime = 0;

    /* Add the particles. */
    for (Particle particle: particles) {
        system.add(particle);
    }
    EXPECT_EQUAL(system.numParticles(), 5);

    /* Move them all. */
    system.moveParticles();
    EXPECT_EQUAL(system.numParticles(), 4); // One was removed

    /* See where they are. */
    ParticleCatcher catcher;
    system.drawParticles();

    /* Confirm we have particles 1, 2, 3, and 4, in that order. */
    EXPECT_EQUAL(catcher.numDrawn(), 4);
    EXPECT_EQUAL(catcher[0], { 1, 1, colors[1] });
    EXPECT_EQUAL(catcher[1], { 2, 1, colors[2] });
    EXPECT_EQUAL(catcher[2], { 3, 1, colors[3] });
    EXPECT_EQUAL(catcher[3], { 4, 1, colors[4] });

    /* Check the linked list to make sure the wiring is right. */
    ParticleSystem::ParticleCell* curr = system._head;
    ParticleSystem::ParticleCell* prev = nullptr;
    for (int i = 0; i < 4; i++) {
        EXPECT_NOT_EQUAL(curr, nullptr);
        EXPECT_EQUAL(curr->prev, prev);

        prev = curr;
        curr = curr->next;
    }
    EXPECT_EQUAL(curr, nullptr);
}

PROVIDED_TEST("Milestone 4: All particles move even if second needs to be removed.") {
    /* Create a list of five particles at positions (0, 0), (1, 0), ... (4, 0).
     * All particles move downward. All particles have a long lifetime except
     * for the second, which has a lifetime of 0 and thus disappears as soon as
     * it moves. We should see all the others shifted down by one spot.
     */
    ParticleSystem system;

    /* Used to identify particles. */
    const Vector<Color> colors = {
        Color::BLACK, Color::WHITE, Color::RED, Color::GREEN, Color::YELLOW
    };

    /* Initialize the particles. */
    Vector<Particle> particles;
    for (int i = 0; i < 5; i++) {
        Particle particle;
        particle.x = i;
        particle.dx = 0;
        particle.dy = 1;
        particle.lifetime = 100;
        particle.color = colors[i]; // So we can flag it later
        particles += particle;
    }
    particles[1].lifetime = 0;

    /* Add the particles. */
    for (Particle particle: particles) {
        system.add(particle);
    }
    EXPECT_EQUAL(system.numParticles(), 5);

    /* Move them all. */
    system.moveParticles();
    EXPECT_EQUAL(system.numParticles(), 4); // One was removed

    /* See where they are. */
    ParticleCatcher catcher;
    system.drawParticles();

    /* Confirm we have particles 0, 2, 3, and 4, in that order. */
    EXPECT_EQUAL(catcher.numDrawn(), 4);
    EXPECT_EQUAL(catcher[0], { 0, 1, colors[0] });
    EXPECT_EQUAL(catcher[1], { 2, 1, colors[2] });
    EXPECT_EQUAL(catcher[2], { 3, 1, colors[3] });
    EXPECT_EQUAL(catcher[3], { 4, 1, colors[4] });

    /* Check the linked list to make sure the wiring is right. */
    ParticleSystem::ParticleCell* curr = system._head;
    ParticleSystem::ParticleCell* prev = nullptr;
    for (int i = 0; i < 4; i++) {
        EXPECT_NOT_EQUAL(curr, nullptr);
        EXPECT_EQUAL(curr->prev, prev);

        prev = curr;
        curr = curr->next;
    }
    EXPECT_EQUAL(curr, nullptr);
}

PROVIDED_TEST("Milestone 4: All particles move even if last needs to be removed.") {
    /* Create a list of five particles at positions (0, 0), (1, 0), ... (4, 0).
     * All particles move downward. All particles have a long lifetime except
     * for the last, which has a lifetime of 0 and thus disappears as soon as
     * it moves. We should see all the others shifted down by one spot.
     */
    ParticleSystem system;

    /* Used to identify particles. */
    const Vector<Color> colors = {
        Color::BLACK, Color::WHITE, Color::RED, Color::GREEN, Color::YELLOW
    };

    /* Initialize the particles. */
    Vector<Particle> particles;
    for (int i = 0; i < 5; i++) {
        Particle particle;
        particle.x = i;
        particle.dx = 0;
        particle.dy = 1;
        particle.lifetime = 100;
        particle.color = colors[i]; // So we can flag it later
        particles += particle;
    }
    particles[4].lifetime = 0;

    /* Add the particles. */
    for (Particle particle: particles) {
        system.add(particle);
    }
    EXPECT_EQUAL(system.numParticles(), 5);

    /* Move them all. */
    system.moveParticles();
    EXPECT_EQUAL(system.numParticles(), 4); // One was removed

    /* See where they are. */
    ParticleCatcher catcher;
    system.drawParticles();

    /* Confirm we have particles 0, 1, 2, and 3, in that order. */
    EXPECT_EQUAL(catcher.numDrawn(), 4);
    EXPECT_EQUAL(catcher[0], { 0, 1, colors[0] });
    EXPECT_EQUAL(catcher[1], { 1, 1, colors[1] });
    EXPECT_EQUAL(catcher[2], { 2, 1, colors[2] });
    EXPECT_EQUAL(catcher[3], { 3, 1, colors[3] });

    /* Check the linked list to make sure the wiring is right. */
    ParticleSystem::ParticleCell* curr = system._head;
    ParticleSystem::ParticleCell* prev = nullptr;
    for (int i = 0; i < 4; i++) {
        EXPECT_NOT_EQUAL(curr, nullptr);
        EXPECT_EQUAL(curr->prev, prev);

        prev = curr;
        curr = curr->next;
    }
    EXPECT_EQUAL(curr, nullptr);
}

PROVIDED_TEST("Milestone 4: All particles move even if second-to-last needs to be removed.") {
    /* Create a list of five particles at positions (0, 0), (1, 0), ... (4, 0).
     * All particles move downward. All particles have a long lifetime except
     * for the penultimate, which has a lifetime of 0 and thus disappears as soon as
     * it moves. We should see all the others shifted down by one spot.
     */
    ParticleSystem system;

    /* Used to identify particles. */
    const Vector<Color> colors = {
        Color::BLACK, Color::WHITE, Color::RED, Color::GREEN, Color::YELLOW
    };

    /* Initialize the particles. */
    Vector<Particle> particles;
    for (int i = 0; i < 5; i++) {
        Particle particle;
        particle.x = i;
        particle.dx = 0;
        particle.dy = 1;
        particle.lifetime = 100;
        particle.color = colors[i]; // So we can flag it later
        particles += particle;
    }
    particles[3].lifetime = 0;

    /* Add the particles. */
    for (Particle particle: particles) {
        system.add(particle);
    }
    EXPECT_EQUAL(system.numParticles(), 5);

    /* Move them all. */
    system.moveParticles();
    EXPECT_EQUAL(system.numParticles(), 4); // One was removed

    /* See where they are. */
    ParticleCatcher catcher;
    system.drawParticles();

    /* Confirm we have particles 0, 1, 2, and 4, in that order. */
    EXPECT_EQUAL(catcher.numDrawn(), 4);
    EXPECT_EQUAL(catcher[0], { 0, 1, colors[0] });
    EXPECT_EQUAL(catcher[1], { 1, 1, colors[1] });
    EXPECT_EQUAL(catcher[2], { 2, 1, colors[2] });
    EXPECT_EQUAL(catcher[3], { 4, 1, colors[4] });

    /* Check the linked list to make sure the wiring is right. */
    ParticleSystem::ParticleCell* curr = system._head;
    ParticleSystem::ParticleCell* prev = nullptr;
    for (int i = 0; i < 4; i++) {
        EXPECT_NOT_EQUAL(curr, nullptr);
        EXPECT_EQUAL(curr->prev, prev);

        prev = curr;
        curr = curr->next;
    }
    EXPECT_EQUAL(curr, nullptr);
}

PROVIDED_TEST("Milestone 4: All particles move even if first needs to be removed.") {
    /* Create a list of five particles at positions (0, 0), (1, 0), ... (4, 0).
     * All particles move downward. All particles have a long lifetime except
     * for the first, which has a lifetime of 0 and thus disappears as soon as
     * it moves. We should see all the others shifted down by one spot.
     */
    ParticleSystem system;

    /* Used to identify particles. */
    const Vector<Color> colors = {
        Color::BLACK, Color::WHITE, Color::RED, Color::GREEN, Color::YELLOW
    };

    /* Initialize the particles. */
    Vector<Particle> particles;
    for (int i = 0; i < 5; i++) {
        Particle particle;
        particle.x = i;
        particle.dx = 0;
        particle.dy = 1;
        particle.lifetime = 100;
        particle.color = colors[i]; // So we can flag it later
        particles += particle;
    }
    particles[2].lifetime = 0;

    /* Add the particles. */
    for (Particle particle: particles) {
        system.add(particle);
    }
    EXPECT_EQUAL(system.numParticles(), 5);

    /* Move them all. */
    system.moveParticles();
    EXPECT_EQUAL(system.numParticles(), 4); // One was removed

    /* See where they are. */
    ParticleCatcher catcher;
    system.drawParticles();

    /* Confirm we have particles 0, 1, 3, and 4, in that order. */
    EXPECT_EQUAL(catcher.numDrawn(), 4);
    EXPECT_EQUAL(catcher[0], { 0, 1, colors[0] });
    EXPECT_EQUAL(catcher[1], { 1, 1, colors[1] });
    EXPECT_EQUAL(catcher[2], { 3, 1, colors[3] });
    EXPECT_EQUAL(catcher[3], { 4, 1, colors[4] });

    /* Check the linked list to make sure the wiring is right. */
    ParticleSystem::ParticleCell* curr = system._head;
    ParticleSystem::ParticleCell* prev = nullptr;
    for (int i = 0; i < 4; i++) {
        EXPECT_NOT_EQUAL(curr, nullptr);
        EXPECT_EQUAL(curr->prev, prev);

        prev = curr;
        curr = curr->next;
    }
    EXPECT_EQUAL(curr, nullptr);
}

PROVIDED_TEST("Milestone 4: All particles move even if many need to be removed.") {
    /* Create a list of five particles at positions (0, 0), (1, 0), ... (4, 0).
     * All particles move downward. Particles 0, 2, and 4 have lifetime 0 and
     * thus will disappear after the first step. The other two will live for
     * multiple time steps.
     */
    ParticleSystem system;

    /* Used to identify particles. */
    const Vector<Color> colors = {
        Color::BLACK, Color::WHITE, Color::RED, Color::GREEN, Color::YELLOW
    };

    /* Initialize the particles. */
    Vector<Particle> particles;
    for (int i = 0; i < 5; i++) {
        Particle particle;
        particle.x = i;
        particle.dx = 0;
        particle.dy = 1;
        particle.lifetime = 100;
        particle.color = colors[i]; // So we can flag it later
        particles += particle;
    }
    particles[0].lifetime = 0;
    particles[2].lifetime = 0;
    particles[4].lifetime = 0;

    /* Add the particles. */
    for (Particle particle: particles) {
        system.add(particle);
    }
    EXPECT_EQUAL(system.numParticles(), 5);

    /* Move them all. */
    system.moveParticles();
    EXPECT_EQUAL(system.numParticles(), 2); // Three were removed

    /* See where they are. */
    ParticleCatcher catcher;
    system.drawParticles();

    /* Confirm we have particles 1 and 3, in that order. */
    EXPECT_EQUAL(catcher.numDrawn(), 2);
    EXPECT_EQUAL(catcher[0], { 1, 1, colors[1] });
    EXPECT_EQUAL(catcher[1], { 3, 1, colors[3] });

    /* Check the linked list to make sure the wiring is right. */
    ParticleSystem::ParticleCell* curr = system._head;
    ParticleSystem::ParticleCell* prev = nullptr;
    for (int i = 0; i < 2; i++) {
        EXPECT_NOT_EQUAL(curr, nullptr);
        EXPECT_EQUAL(curr->prev, prev);

        prev = curr;
        curr = curr->next;
    }
    EXPECT_EQUAL(curr, nullptr);
}

PROVIDED_TEST("Milestone 4: After all particles expire, can add new particles.") {
    ParticleSystem system;

    /* On entry to this loop, the particle system should be empty.
     * On exit, it should be empty.
     */
    for (int round = 0; round < 10; round++) {
        /* Shouldn't see anything. */
        ParticleCatcher catcher;
        system.drawParticles();
        EXPECT_EQUAL(catcher.numDrawn(), 0);

        /* Create a bunch of short-lived particles. */
        for (int i = 0; i < 5; i++) {
            Particle p;
            p.x = i;
            p.y = i;
            p.lifetime = 2;
            system.add(p);
        }
        EXPECT_EQUAL(system.numParticles(), 5);

        /* Should see five particles. */
        system.drawParticles();
        EXPECT_EQUAL(catcher.numDrawn(), 5);

        /* They should be in the right place. */
        for (int i = 0; i < 5; i++) {
            EXPECT_EQUAL(catcher[i].x, i);
            EXPECT_EQUAL(catcher[i].y, i);
        }

        /* Move the particles a bunch to clear everything out as their
         * lifetimes expire.
         */
        for (int i = 0; i < 10; i++) {
            system.moveParticles();
        }

        /* We should have no particles. */
        EXPECT_EQUAL(system.numParticles(), 0);
        catcher.reset();
        system.drawParticles();
        EXPECT_EQUAL(catcher.numDrawn(), 0);
    }
}

PROVIDED_TEST("Milestone 5: Ballistic particles accelerate downward.") {
    ParticleSystem system;

    Particle ballistic;
    ballistic.type = ParticleType::BALLISTIC;
    ballistic.x = 0;
    ballistic.y = 100;

    /* Fire upward and to the right. */
    ballistic.dx = 1;
    ballistic.dy = -10;

    system.add(ballistic);

    /* Expected positions. */
    Vector<GPoint> positions = {
        {  0, 100 },
        {  1,  90 }, // y += -10
        {  2,  81 }, // y += -9
        {  3,  73 }, // y += -8
        {  4,  66 },
        {  5,  60 },
        {  6,  55 },
        {  7,  51 },
        {  8,  48 },
        {  9,  46 },
        { 10,  45 },
        { 11,  45 }, // y += 0
        { 12,  46 },
        { 13,  48 },
        { 14,  51 },
        { 15,  55 },
        { 16,  60 },
        { 17,  66 },
        { 18,  73 },
        { 19,  81 }, // y += 8
        { 20,  90 }, // y += 9
        { 21, 100 }, // y += 10
    };

    /* Repeatedly check positions and move forward. */
    for (int i = 0; i < positions.size(); i++) {
        ParticleCatcher catcher;
        system.drawParticles();
        EXPECT_EQUAL(catcher.numDrawn(), 1);
        EXPECT_EQUAL(catcher[0].x, positions[i].x);
        EXPECT_EQUAL(catcher[0].y, positions[i].y);

        system.moveParticles();
    }
}

PROVIDED_TEST("Milestone 5: Fireworks accelerate downward.") {
    ParticleSystem system;

    Particle firework;
    firework.type = ParticleType::FIREWORK;
    firework.x = 0;
    firework.y = 100;

    /* Fire upward and to the right. */
    firework.dx = 1;
    firework.dy = -10;

    system.add(firework);

    /* Expected positions. */
    Vector<GPoint> positions = {
        {  0, 100 },
        {  1,  90 }, // y += -10
        {  2,  81 }, // y += -9
        {  3,  73 }, // y += -8
        {  4,  66 },
        {  5,  60 },
        {  6,  55 },
        {  7,  51 },
        {  8,  48 },
        {  9,  46 },
        { 10,  45 },
        { 11,  45 }, // y += 0
        { 12,  46 },
        { 13,  48 },
        { 14,  51 },
        { 15,  55 },
        { 16,  60 },
        { 17,  66 },
        { 18,  73 },
        { 19,  81 }, // y += 8
        { 20,  90 }, // y += 9
        { 21, 100 }, // y += 10
    };

    /* Repeatedly check positions and move forward. */
    for (int i = 0; i < positions.size(); i++) {
        ParticleCatcher catcher;
        system.drawParticles();
        EXPECT_EQUAL(catcher.numDrawn(), 1);
        EXPECT_EQUAL(catcher[0].x, positions[i].x);
        EXPECT_EQUAL(catcher[0].y, positions[i].y);

        system.moveParticles();
    }
}

PROVIDED_TEST("Milestone 5: Fireworks explode when timer expires.") {
    ParticleSystem system;

    /* Create and add a firework. */
    Particle firework;
    firework.type = ParticleType::FIREWORK;
    firework.lifetime = 0; // Explodes as soon as it moves
    firework.x = 100;
    firework.y = 100;
    system.add(firework);

    EXPECT_EQUAL(system.numParticles(), 1);

    /* Move the particles. This should trigger the explosion. */
    system.moveParticles();

    /* There should be 50 new particles, all of which are streamers, and
     * all of which are the same color.
     */
    EXPECT_EQUAL(system.numParticles(), 50);
    EXPECT_NOT_EQUAL(system._head, nullptr);

    /* Color of the first particle. */
    Color color = system._head->particle.color;
    for (auto* curr = system._head; curr != nullptr; curr = curr->next) {
        EXPECT_EQUAL(curr->particle.color, color);
    }
}
