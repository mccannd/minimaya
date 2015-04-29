#include <material.h>



Material::Material(float rflec, float rfrac, float alpha, float specl) {
  this->rflec = rflec;
  this->rfrac = rfrac;
  this->alpha = alpha;
  this->specl = specl; 
}

const Material Material::GLASS   = Material(0.00f, 4.00f, 0.05f, 30.0f);
const Material Material::MIRROR  = Material(1.00f, 1.50f, 1.00f, 30.0f);
const Material Material::PLASTIC = Material(0.05f, 1.00f, 1.00f, 20.0f);
const Material Material::RUBBER  = Material(0.00f, 1.00f, 1.00f, 5.00f);
const Material Material::PAPER   = Material(0.00f, 1.00f, 0.05f, 1.00f);