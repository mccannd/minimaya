#include <material.h>



Material::Material(float rflec, float rfrac, float alpha, float specl) {
  this->rflec = rflec;
  this->rfrac = rfrac;
  this->alpha = alpha;
  this->specl = specl; 
}

const Material Material::GLASS = Material(0.25f, 1.5, 0.01f, 30.0f);
const Material Material::MIRROR = Material(1.0f, 1.5, 1.0f, 30.0f);
const Material Material::PLASTIC = Material(0.05f, 1.0, 1.0f, 20.0f);
const Material Material::RUBBER = Material(0.0f, 1.0, 1.0f, 10.0f);
const Material Material::PAPER = Material(0.0f, 1.0, 0.05f, 5.0f);