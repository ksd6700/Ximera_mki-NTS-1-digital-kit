/*
 * Ximera MKI - NTS-1 digital kit oscillator
 *
 * A compact virtual-analog voice that blends wide digital lead stacks,
 * bright VA square/saw color, and warm driven sub weight.
 */

#include "userosc.h"
#include "biquad.hpp"

namespace {

static inline float wrap_phase(float x) {
  return x - static_cast<uint32_t>(x);
}

static inline float bipolar_percent(uint16_t value) {
  return clipminmaxf(-1.f, (static_cast<int32_t>(value) - 100) * 0.01f, 1.f);
}

struct Ximera {
  enum {
    kNumVoices = 7
  };

  struct Params {
    float detune;
    float spread;
    float sub;
    float drive;
    float tone;
    float shape;
    float shift_shape;
    uint8_t mode;

    Params()
        : detune(0.32f),
          spread(0.58f),
          sub(0.34f),
          drive(0.22f),
          tone(0.18f),
          shape(0.52f),
          shift_shape(0.36f),
          mode(1) {}
  };

  Params params;
  float phase[kNumVoices];
  float sub_phase;
  float ratio[kNumVoices];
  dsp::BiQuad tone_lpf;
  dsp::BiQuad dc_block;

  Ximera() {
    init();
  }

  void init() {
    params = Params();
    for (uint8_t i = 0; i < kNumVoices; ++i) {
      phase[i] = 0.f;
      ratio[i] = 1.f;
    }
    sub_phase = 0.f;
    tone_lpf.flush();
    dc_block.flush();
    tone_lpf.mCoeffs.setSOLP(osc_tanpif(0.18f), 1.2f);
    dc_block.mCoeffs.setFODC(0.995f);
    update_detune();
  }

  void reset_phases() {
    static const float kSeeds[kNumVoices] = {
        0.000f, 0.113f, 0.271f, 0.397f, 0.541f, 0.709f, 0.863f};
    for (uint8_t i = 0; i < kNumVoices; ++i) {
      phase[i] = wrap_phase(kSeeds[i] * params.spread);
    }
    sub_phase = 0.f;
    tone_lpf.flush();
    dc_block.flush();
  }

  void update_detune() {
    static const float kOffsets[kNumVoices] = {
        0.f, -1.f, 1.f, -0.63f, 0.63f, -0.28f, 0.28f};

    const float cents = 1.5f + 28.f * params.detune * params.detune;
    for (uint8_t i = 0; i < kNumVoices; ++i) {
      ratio[i] = fasterpow2f(kOffsets[i] * cents * 0.000833333333f);
    }
  }
};

Ximera s_ximera;

}  // namespace

void OSC_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  s_ximera.init();
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  Ximera::Params &p = s_ximera.params;

  const uint8_t note = params->pitch >> 8;
  const uint8_t fine = params->pitch & 0xff;
  const float note_f = note + fine * 0.00390625f;
  const float base_w0 = osc_w0f_for_note(note, fine);
  const float saw_idx = clipminmaxf(0.f, osc_bl_saw_idx(note_f), 5.999f);
  const float sqr_idx = clipminmaxf(0.f, osc_bl_sqr_idx(note_f), 5.999f);
  const float par_idx = clipminmaxf(0.f, osc_bl_par_idx(note_f), 5.999f);

  const float lfo_shape = q31_to_f32(params->shape_lfo) * 0.22f;
  const float macro = clip01f(p.shape + lfo_shape);
  const float edge = clip01f(p.shift_shape);
  const float cutoff = clip01f(params->cutoff * 0.000122085215f);
  const float resonance = clip01f(params->resonance * 0.000122085215f);

  float tone = 0.22f + 0.27f * (p.tone * 0.5f + 0.5f) + 0.09f * macro + 0.13f * cutoff;
  if (p.mode == 2) {
    tone *= 0.68f;
  } else if (p.mode == 1) {
    tone *= 1.08f;
  }
  tone = clipminmaxf(0.018f, tone, 0.48f);

  const float q = 1.05f + resonance * (2.0f + (p.mode == 2 ? 1.0f : 0.0f));
  s_ximera.tone_lpf.mCoeffs.setSOLP(osc_tanpif(tone), q);

  const float side_gain = 0.045f + 0.12f * p.spread + 0.025f * macro;
  const float super_norm = 1.f / (0.52f + 6.f * side_gain);
  const float sub_gain = 0.12f + 0.55f * p.sub;
  const float drive = 0.92f + 1.65f * p.drive;
  const float drive_trim = 0.78f - 0.12f * p.drive;

  float lead_w = 0.34f + 0.18f * (1.f - macro);
  float jp_w = 0.24f + 0.34f * macro;
  float mini_w = 0.30f + 0.18f * (1.f - macro);

  if (p.mode == 0) {
    lead_w += 0.25f;
  } else if (p.mode == 1) {
    jp_w += 0.30f;
  } else {
    mini_w += 0.28f;
  }

  const float weight_norm = 1.f / (lead_w + jp_w + mini_w);

  q31_t *__restrict y = reinterpret_cast<q31_t *>(yn);
  const q31_t *const y_e = y + frames;

  while (y != y_e) {
    const float saw0 = osc_bl2_sawf(s_ximera.phase[0], saw_idx);
    const float sqr0 = osc_bl2_sqrf(wrap_phase(s_ximera.phase[0] + (edge - 0.5f) * 0.018f), sqr_idx);
    const float par0 = osc_bl2_parf(s_ximera.phase[0], par_idx);

    float super = 0.52f * saw0;
    for (uint8_t i = 1; i < Ximera::kNumVoices; ++i) {
      super += side_gain * osc_bl2_sawf(s_ximera.phase[i], saw_idx);
    }
    super *= super_norm;

    const float sub_square = osc_bl2_sqrf(s_ximera.sub_phase, sqr_idx);
    const float sub_sine = osc_sinf(s_ximera.sub_phase);
    const float sub = 0.70f * sub_square + 0.30f * sub_sine;

    const float lead = 0.64f * saw0 + 0.28f * sqr0 + 0.08f * par0;
    const float mini = (0.78f * saw0 + sub_gain * sub) / (0.78f + sub_gain);
    float sig = (lead_w * lead + jp_w * super + mini_w * mini) * weight_norm;

    sig = s_ximera.tone_lpf.process_so(sig);
    sig = osc_softclipf(0.29f, sig * drive) * drive_trim;
    sig = s_ximera.dc_block.process_fo(sig);

    *(y++) = f32_to_q31(clip1m1f(sig * 0.92f));

    for (uint8_t i = 0; i < Ximera::kNumVoices; ++i) {
      s_ximera.phase[i] = wrap_phase(s_ximera.phase[i] + base_w0 * s_ximera.ratio[i]);
    }
    s_ximera.sub_phase = wrap_phase(s_ximera.sub_phase + base_w0 * 0.5f);
  }
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  (void)params;
  s_ximera.reset_phases();
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  Ximera::Params &p = s_ximera.params;

  switch (index) {
    case k_user_osc_param_id1:
      p.detune = clip01f(value * 0.01f);
      s_ximera.update_detune();
      break;

    case k_user_osc_param_id2:
      p.spread = clip01f(value * 0.01f);
      break;

    case k_user_osc_param_id3:
      p.sub = clip01f(value * 0.01f);
      break;

    case k_user_osc_param_id4:
      p.drive = clip01f(value * 0.01f);
      break;

    case k_user_osc_param_id5:
      p.tone = bipolar_percent(value);
      break;

    case k_user_osc_param_id6:
      p.mode = static_cast<uint8_t>(value > 2 ? 2 : value);
      break;

    case k_user_osc_param_shape:
      p.shape = param_val_to_f32(value);
      break;

    case k_user_osc_param_shiftshape:
      p.shift_shape = param_val_to_f32(value);
      break;

    default:
      break;
  }
}
