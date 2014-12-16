#include <Randomizer.h>

Randomizer::Randomizer()
        : rng(std::random_device()()) {
}

double Randomizer::randomUnit() {
    return std::generate_canonical<double, 16>(rng);
}

double Randomizer::randomSignedUnit() {
    return 2.0 * (0.5 * randomUnit() - 0.5);
}

Randomizer::~Randomizer() {
}