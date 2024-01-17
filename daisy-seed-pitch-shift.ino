#include "DaisyDuino.h"
#include "simple-daisy.h"


// Setup pins
static const int mix_pin   = A(S30);
static const int pitch_pin = A(S31);
static const int del_pin   = A(S32);
static const int fun_pin   = A(S33);


static const float kKnobMax = 1023;

float mix_control = 0;


static PitchShifter pitch_shifter;
static CrossFade cfade;

void AudioCallback(float **in, float **out, size_t size) {
    float shifted, unshifted, mix;
    // float mix_control = 0.25;
    for (size_t i = 0; i < size; i++) {
        unshifted = in[1][i];
        shifted = pitch_shifter.Process(unshifted);
        mix = mix_control * shifted + (1 - mix_control) * unshifted;
        out[0][i] = out[1][i] = mix;
    }

}

void setup() {
    DAISY.init(DAISY_SEED, AUDIO_SR_48K);
    float sample_rate = DAISY.get_samplerate();
    pitch_shifter.Init(sample_rate);
    DAISY.begin(AudioCallback);
    Serial.begin(9600);
}

void loop() {
    //get controls 
    mix_control = fmap(analogRead(mix_pin) / kKnobMax, 0.f, 1.f);
    auto pitch_val = fmap(analogRead(pitch_pin) / kKnobMax, 0.f, 1.f);
    uint32_t del_value = fmap(analogRead(del_pin) / kKnobMax, 1.f, 20000.f);
    float fun_value = fmap(analogRead(fun_pin) / kKnobMax, 0.f, 0.3);
    Serial.println(fun_value);

    auto transposition = 24.0 * (pitch_val - 0.5);
    pitch_shifter.SetTransposition(transposition);
    pitch_shifter.SetDelSize(del_value);
    pitch_shifter.SetFun(fun_value);
}