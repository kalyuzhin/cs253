//
// Created by Марк Калюжин on 02.12.2025.
//

#ifndef COMPUTINGSYSTEMS_RACE_H
#define COMPUTINGSYSTEMS_RACE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

using namespace std;

using ll = long long;

struct Vec2 {
    double x, y;

    Vec2(double x = 0.0, double y = 0.0) : x(x), y(y) {}

    double distance_to(const Vec2 &other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return sqrt(dx * dx + dy * dy);
    }
};

struct Gene {
    double delta_angle;
    ll thrust;
};

struct PodState {
    Vec2 position;
    Vec2 velocity;
    double angle;
};

double degrees_to_radians(double degrees) {
    return degrees * M_PI / 180.0;
}

const ll POPULATION_SIZE = 30;
const ll SEQUENCE_LENGTH = 6;
const ll GENERATION_COUNT = 20;
const double MUTATION_PROBABILITY = 0.1;

random_device rd;
mt19937 rng(rd());

uniform_real_distribution<double> angle_distribution(-18.0, 18.0);
uniform_int_distribution<ll> thrust_distribution(0, 200);
uniform_real_distribution<double> mutation_chance_distribution(0.0, 1.0);
uniform_int_distribution<ll> parent_index_distribution(0, 9);
uniform_int_distribution<ll> crossover_point_distribution(0, SEQUENCE_LENGTH - 1);

vector<Gene> make_random_sequence() {
    vector<Gene> sequence(SEQUENCE_LENGTH);
    for (auto &gene: sequence) {
        gene.delta_angle = angle_distribution(rng);
        gene.thrust = thrust_distribution(rng);
    }
    return sequence;
}

PodState run_simulation(const PodState &start_state, const Vec2 &checkpoint, const vector<Gene> &sequence) {
    PodState current = start_state;

    for (const Gene &gene: sequence) {
        current.angle += gene.delta_angle;
        double rad = degrees_to_radians(current.angle);

        current.velocity.x += cos(rad) * static_cast<double>(gene.thrust);
        current.velocity.y += sin(rad) * static_cast<double>(gene.thrust);

        current.velocity.x *= 0.85;
        current.velocity.y *= 0.85;

        current.position.x += current.velocity.x;
        current.position.y += current.velocity.y;
    }

    return current;
}

double compute_fitness(const PodState &final_state, const Vec2 &checkpoint) {
    return -final_state.position.distance_to(checkpoint);
}

vector<Gene> crossover(const vector<Gene> &parent_a, const vector<Gene> &parent_b) {
    vector<Gene> child(SEQUENCE_LENGTH);
    ll cut_point = crossover_point_distribution(rng);

    for (ll i = 0; i < SEQUENCE_LENGTH; ++i) {
        if (i < cut_point) {
            child[i] = parent_a[i];
        } else {
            child[i] = parent_b[i];
        }
    }

    return child;
}

void mutate(vector<Gene> &sequence) {
    for (auto &gene: sequence) {
        if (mutation_chance_distribution(rng) < MUTATION_PROBABILITY) {
            gene.delta_angle += angle_distribution(rng) * 0.5;
        }
        if (mutation_chance_distribution(rng) < MUTATION_PROBABILITY) {
            gene.thrust += (thrust_distribution(rng) - 100) / 5;
        }
        gene.thrust = max(0LL, min(200LL, gene.thrust));
    }
}

int run_race() {
    ll checkpoints_count;
    cin >> checkpoints_count;

    vector<Vec2> checkpoints(checkpoints_count);
    for (ll i = 0; i < checkpoints_count; ++i) {
        cin >> checkpoints[i].x >> checkpoints[i].y;
    }

    for (;;) {
        ll checkpoint_index;
        ll x, y, vx, vy, angle;
        cin >> checkpoint_index >> x >> y >> vx >> vy >> angle;

        PodState pod{
                Vec2(static_cast<double>(x), static_cast<double>(y)),
                Vec2(static_cast<double>(vx), static_cast<double>(vy)),
                static_cast<double>(angle)
        };

        Vec2 current_checkpoint = checkpoints[checkpoint_index];

        vector<vector<Gene>> population(POPULATION_SIZE);
        for (auto &chromosome: population) {
            chromosome = make_random_sequence();
        }

        for (ll generation = 0; generation < GENERATION_COUNT; ++generation) {
            vector<pair<double, ll>> fitness_with_index;
            fitness_with_index.reserve(POPULATION_SIZE);

            for (ll i = 0; i < POPULATION_SIZE; ++i) {
                PodState result_state = run_simulation(pod, current_checkpoint, population[i]);
                double fitness = compute_fitness(result_state, current_checkpoint);
                fitness_with_index.emplace_back(fitness, i);
            }

            sort(fitness_with_index.rbegin(), fitness_with_index.rend());

            vector<vector<Gene>> new_population;
            new_population.reserve(POPULATION_SIZE);

            for (ll i = 0; i < 5; ++i) {
                new_population.push_back(population[fitness_with_index[i].second]);
            }

            while (static_cast<ll>(new_population.size()) < POPULATION_SIZE) {
                ll parent_a_idx = parent_index_distribution(rng);
                ll parent_b_idx = parent_index_distribution(rng);

                const vector<Gene> &parent_a = population[fitness_with_index[parent_a_idx].second];
                const vector<Gene> &parent_b = population[fitness_with_index[parent_b_idx].second];

                vector<Gene> child = crossover(parent_a, parent_b);
                mutate(child);
                new_population.push_back(child);
            }

            population.swap(new_population);
        }

        double best_fitness = -1e18;
        vector<Gene> best_sequence;

        for (const auto &chromosome: population) {
            PodState final_state = run_simulation(pod, current_checkpoint, chromosome);
            double fitness = compute_fitness(final_state, current_checkpoint);
            if (fitness > best_fitness) {
                best_fitness = fitness;
                best_sequence = chromosome;
            }
        }

        const Gene &first_action = best_sequence[0];
        double final_angle = pod.angle + first_action.delta_angle;
        double rad = degrees_to_radians(final_angle);

        ll target_x = static_cast<ll>(pod.position.x + cos(rad) * 1000.0);
        ll target_y = static_cast<ll>(pod.position.y + sin(rad) * 1000.0);

        cout << target_x << " " << target_y << " " << first_action.thrust << " message" << endl;
    }
}


#endif //COMPUTINGSYSTEMS_RACE_H
