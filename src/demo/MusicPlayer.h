#pragma once

class VorbisPlayer
{
  public:
    VorbisPlayer();
    ~VorbisPlayer();

    void play(const char *fileName);

    double getTime() const;

  private:
    struct Impl;
    Impl *m_impl = nullptr;
};
