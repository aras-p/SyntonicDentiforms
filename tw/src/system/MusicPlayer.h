#pragma once

class CMusicPlayer {
public:
	CMusicPlayer();
	~CMusicPlayer();

	void play( const char* fileName );

	double getTime() const;

private:
	struct Impl;
	Impl* m_impl = nullptr;
};
