#pragma once

class MusicPlayer
{
  public:
    MusicPlayer();
    ~MusicPlayer();

    void play(const char *fileName);

    double getTime() const;

  private:
    struct Impl;
    Impl *m_impl = nullptr;
};
