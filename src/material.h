#pragma once

class Material {
  public:
    float rflec, rfrac, alpha, specl;

    Material(float rflec, float rfrac, float alpha, float specl);

    static const Material GLASS;
    static const Material MIRROR;
    static const Material PLASTIC;
    static const Material RUBBER;
    static const Material PAPER;
};