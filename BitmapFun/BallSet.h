#pragma once
#include <Windows.h>
#include <stdint.h>

#define BALL_MAX_RADIUS (32)
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define H_ELASTIC_EFFICIENCY (0.90)
#define V_ELASTIC_EFFICIENCY (0.80)
#define H_FRICTION (0.97)

#define PI  (3.141592653589)
#define WHITE (0x00FFFFFF)
#define BLACK (0x00000000)
#define RED   (0x00FF0000)
#define GREEN (0x0000FF00)
#define BLUE  (0x000000FF)

class BallSet {
public:
	BallSet(int numberOfBalls, int W, int H);
	~BallSet();
	void draw(HDC hdc, int W, int H);

private:

	//// GREEN BALL BITMAP
	uint32_t ** ballArray;
	uint32_t ** ballArrayMask;
	BITMAPINFO * ballBmpInfo;
	HPEN penTightString;
	HPEN penLooseString;

	int N;

	double * ballX;
	double * ballY;
	double * ballXVelocity;
	double * ballYVelocity;
	double * ballMass;
	double * ballRadius;
	int * ballIntDiameter;
	char * ballVsBallUpdate;
	double * stringAnchorX;
	double * stringAnchorY;
	double * stringLength;
	char * stringIsTight;

	double ballGravity = 0.01;

	void updatePosition(int W, int H);
	void handleCollisions(void);



};