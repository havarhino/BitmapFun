
#include "stdafx.h"
#include "BallSet.h"
#include <math.h>

#undef NEWTONS_CRADLE

BallSet::BallSet(int numberOfBalls, int W, int H) {

#ifdef NEWTONS_CRADLE
	N = 6;
#else
	N = numberOfBalls;
#endif

	double ballDensity = 0.0001;

	penTightString = CreatePen(PS_SOLID, 2, RGB(192, 0, 0));
	penLooseString = CreatePen(PS_SOLID, 2, RGB(0, 128, 0));

	ballX = new double[N];
	ballY = new double[N];
	ballXVelocity = new double[N];
	ballYVelocity = new double[N];
	ballRadius = new double[N];
	ballMass = new double[N];
	ballIntDiameter = new int[N];
	stringAnchorX = new double[N];
	stringAnchorY = new double[N];
	stringLength = new double[N];
	stringIsTight = new char[N];

	srand(133);

	ballArray = new uint32_t*[N];
	ballArrayMask = new uint32_t*[N];
	ballBmpInfo = new BITMAPINFO[N];
	ballVsBallUpdate = new char[N*N];

	double totalBallWidth = 0.0;
	for (int i = 0; i < N; i++) {
#ifdef NEWTONS_CRADLE
		ballRadius[i] = 30.0;
#else
		ballRadius[i] = rand() % (BALL_MAX_RADIUS - 5) + 5;
#endif
		ballIntDiameter[i] = (int)(2.0 * ballRadius[i] + 0.99999);

		totalBallWidth += ballRadius[i] * 2.0;
	}

	double maxStringLength = (W - totalBallWidth) / 2.0 * 0.95;

	for (int i = 0; i < N; i++) {
		int D = ballIntDiameter[i];
#ifdef NEWTONS_CRADLE
		ballMass[i] = 4.0 / 3.0 * PI * ballRadius[i] * ballRadius[i] * ballRadius[i] * ballDensity;

		int x = (W - totalBallWidth)/2.0 + ballRadius[0] + D*i;

		//stringLength[i] = 230.0 + (i - N/2)*14;
		//stringLength[i] = 300.0;
		stringLength[i] = maxStringLength;

		//stringAnchorX[i] = x + (i - N/2)*10;
		stringAnchorX[i] = x;
		stringAnchorY[i] = 10;

		ballXVelocity[i] = 0.0;
		ballYVelocity[i] = 0.0;

		if (i == N - 1) {
			ballX[i] = x + stringLength[i] - ballRadius[i];
			ballY[i] = stringAnchorY[i];
		}
		else if (i == N - 2) {
			ballX[i] = x + stringLength[i]/2 - ballRadius[i];
			ballY[i] = stringAnchorY[i];
		}
		else {
			ballX[i] = x - ballRadius[i];
			ballY[i] = stringAnchorY[i] + stringLength[i];
		}
#else
		ballX[i] = rand() % (W - D);
		ballY[i] = rand() % (H / 2 - D);
		double MAX_H_VELOCITY = 2.3;
		int tmpV = (int)(MAX_H_VELOCITY * 10.0);
		ballXVelocity[i] = (double)(rand() % tmpV - tmpV / 2) / 10.0;
		ballYVelocity[i] = 0.0;
		ballMass[i] = 4.0 / 3.0 * PI * ballRadius[i] * ballRadius[i] * ballRadius[i] * ballDensity;
		//ballMass[i] = 1.0;

		stringAnchorX[i] = rand() % W;
		stringAnchorY[i] = rand() % (int)(H / 2.0);

		double percentObjectsOnString = 0.50;
		if( rand() % 100  < (int)(100.0 * percentObjectsOnString) ) {
		    stringLength[i] = rand() % (int)((H / 2.0)) + 20;
		}
		else {
			stringLength[i] = -1.0;
		}
#endif

		ballArray[i] = new uint32_t[D*D];
		ballArrayMask[i] = new uint32_t[D*D];
		for (int y = 0; y < D; y++) {
			for (int x = 0; x < D; x++) {
				double cx = (double)x - ballRadius[i];
				double cy = (double)y - ballRadius[i];
				if (cx*cx + cy*cy > ballRadius[i] * ballRadius[i]) {
					ballArray[i][y*D + x] = BLACK;
					ballArrayMask[i][y*D + x] = WHITE;
				}
				else {
					ballArray[i][y*D + x] = GREEN;
					ballArrayMask[i][y*D + x] = BLACK;
				}
			}
		}
		ballBmpInfo[i].bmiHeader.biSize = sizeof(BITMAPINFO);
		ballBmpInfo[i].bmiHeader.biWidth = D;
		ballBmpInfo[i].bmiHeader.biHeight = D;
		ballBmpInfo[i].bmiHeader.biBitCount = 32;
		ballBmpInfo[i].bmiHeader.biCompression = BI_RGB;
		ballBmpInfo[i].bmiHeader.biPlanes = 1;
		ballBmpInfo[i].bmiHeader.biSizeImage = D * D * 4;

	}
}

BallSet::~BallSet() {
	DeleteObject(penTightString);
	DeleteObject(penLooseString);
	penTightString = NULL;
	penLooseString = NULL;
}

void BallSet::handleCollisions(void) {

	memset(ballVsBallUpdate, 0, N*N * sizeof(char));

	for (int i = 0; i < N; i++) {
		double X1 = ballX[i] + ballRadius[i];
		double Y1 = ballY[i] + ballRadius[i];
		double V1X = ballXVelocity[i];
		double V1Y = ballYVelocity[i];

		for (int k = 0; k < N; k++) {
			if ((i != k) && (ballVsBallUpdate[i*N + k] == 0)) {
			//if (i != k) {

				// Set each other's updated reference
				ballVsBallUpdate[i*N + k] = 1;
				ballVsBallUpdate[k*N + i] = 1;

				double X2 = ballX[k] + ballRadius[k];
				double Y2 = ballY[k] + ballRadius[k];
				double V2X = ballXVelocity[k];
				double V2Y = ballYVelocity[k];

				double dX = X1 - X2;
				double dY = Y1 - Y2;

				double ballSeparation = ballRadius[i] + ballRadius[k];


				// If balls are not touching, skip
				double distSquared = dX*dX + dY*dY;

				if (distSquared >= ballSeparation*ballSeparation) {
					continue;
				}

				// Now, see if the balls are getting closer, or moving apart
				double newX1 = X1 + V1X;
				double newY1 = Y1 + V1Y;
				double newX2 = X2 + V2X;
				double newY2 = Y2 + V2Y;

				double newDX = newX1 - newX2;
				double newDY = newY1 - newY2;
				double newDistSquared = newDX*newDX + newDY*newDY;

				if (newDistSquared >= distSquared) {
					continue;
				}

				double lineX, lineY;
				double linePerpX, linePerpY;

				// Now, compute the line connecting center points
				// If circles are completely on top of each other, use velocity vectors to fine line
				if ((X1 == X2) && (Y1 == Y2)) {
					// First, find line that is the sum of the two velocity vectors
					linePerpX = X1 + X2;
					linePerpY = Y1 + Y2;
					double mag = sqrt(linePerpX*linePerpX + linePerpY*linePerpY);
					linePerpX /= mag;
					linePerpY /= mag;

					// Now, the line we want is perpendicular to the sum
					lineX = -linePerpY;
					lineY = linePerpX;
				}
				else {
					lineX = X1 - X2;
					lineY = Y1 - Y2;
					double mag = sqrt(lineX*lineX + lineY*lineY);
					lineX /= mag;
					lineY /= mag;
					linePerpX = -lineY;
					linePerpY = lineX;
				}

				// Now, find velocity components of each ball along  line of interaction
				//  (V1X * x + V1Y * y) . (lineX * x + lineY * y) / |line| * line_vector
				//  (V1X * x + V1Y * y) . (lineX * x + lineY * y)  * line_vector
				// (V1X * lineX + V1Y * lineY) * line_vector
				double dotProduct1 = V1X * lineX + V1Y * lineY;
				double dotProduct2 = V2X * lineX + V2Y * lineY;

				double V1AX = dotProduct1 * lineX;
				double V1AY = dotProduct1 * lineY;
				double V2AX = dotProduct2 * lineX;
				double V2AY = dotProduct2 * lineY;

				// Now, find velocity components of each ball along perpendicular line of interaction
				// These will not be changed by the collision.
				dotProduct1 = V1X * linePerpX + V1Y * linePerpY;
				dotProduct2 = V2X * linePerpX + V2Y * linePerpY;

				double V1PX = dotProduct1 * linePerpX;
				double V1PY = dotProduct1 * linePerpY;
				double V2PX = dotProduct2 * linePerpX;
				double V2PY = dotProduct2 * linePerpY;

				double m1 = ballMass[i];
				double m2 = ballMass[k];
				double totalMass = m1 + m2;

				// Collision Efficiency
				double CE = 0.98;
				// Now, apply the equations of ellastic collision
				double V1AXprime = ((m1 - m2) / totalMass) * V1AX*CE + (2.0*m2 / totalMass) * V2AX*CE;
				double V1AYprime = ((m1 - m2) / totalMass) * V1AY*CE + (2.0*m2 / totalMass) * V2AY*CE;

				double V2AXprime = (2.0*m1 / totalMass) * V1AX*CE - ((m1 - m2) / totalMass) * V2AX*CE;
				double V2AYprime = (2.0*m1 / totalMass) * V1AY*CE - ((m1 - m2) / totalMass) * V2AY*CE;

				// Our new total velocity is just the sum of the unchanged perpendicular component
				// and the new collision component: i is the first ball, k is the second
				ballXVelocity[i] = V1PX + V1AXprime;
				ballYVelocity[i] = V1PY + V1AYprime;

				ballXVelocity[k] = V2PX + V2AXprime;
				ballYVelocity[k] = V2PY + V2AYprime;

			}
		}
	}

}

void BallSet::updatePosition(int W, int H) {

	// First, update for influence of gravity and ground
	for (int i = 0; i < N; i++) {
		double X = ballX[i];
		double Y = ballY[i];
		double vX = ballXVelocity[i];
		double vY = ballYVelocity[i];

		X += vX;
		Y += vY;
		vY += ballGravity;

		if (Y >= (H - ballIntDiameter[i])) {
			// How far below ground is ball
			double D = Y - (H - ballIntDiameter[i]);

			// Move ball back to ground level
			Y = H - ballIntDiameter[i];

			// Now, decrease
			vY = sqrt(vY*vY - 2.0*ballGravity*D);

			// Now, reverse direction for collision and apply non-elastic loss
			vY *= -V_ELASTIC_EFFICIENCY;
			vX *= H_FRICTION;
		}

		if (X >= (double)W - ballIntDiameter[i]) {
			X = W - ballIntDiameter[i] - 1;
			vX *= -H_ELASTIC_EFFICIENCY;
		}
		if (X < 0.0) {
			X = 0;
			vX *= -H_ELASTIC_EFFICIENCY;
		}

		ballX[i] = X;
		ballY[i] = Y;
		ballXVelocity[i] = vX;
		ballYVelocity[i] = vY;

		int IS_ROD = 1;

		// Now, constrain to string, if set
		if (stringLength[i] > 0.0) {
			double dRX = ballX[i]+ballRadius[i] - stringAnchorX[i];
			double dRY = ballY[i]+ballRadius[i] - stringAnchorY[i];
			double mag = sqrt(dRX*dRX + dRY*dRY);

			if ( (IS_ROD == 1) || (mag > stringLength[i]) ) {
			    double dRXunit = dRX / mag;
			    double dRYunit = dRY / mag;
			    double dRXunitPerp = dRYunit;
			    double dRYunitPerp = -dRXunit;

				ballX[i] = stringAnchorX[i] + stringLength[i] * dRXunit - ballRadius[i];
				ballY[i] = stringAnchorY[i] + stringLength[i] * dRYunit - ballRadius[i];

			    double dotProductR = vX * dRXunit + vY * dRYunit;
				double dotProductT = vX * dRXunitPerp + vY * dRYunitPerp;

			    double vRX = dotProductR * dRXunit;
			    double vRY = dotProductR * dRYunit;

			    double vTX = dotProductT * dRXunitPerp;
			    double vTY = dotProductT * dRYunitPerp;

				ballXVelocity[i] = vTX;
				ballYVelocity[i] = vTY;

				if ((IS_ROD == 0) && (dotProductR < 0.0)) {
					ballXVelocity[i] += vRX;
					ballYVelocity[i] += vRY;
				}
				if (IS_ROD == 0) {
					double stringBounceEfficiency = 0.5;
					if (dotProductR < 0.0) {
						ballXVelocity[i] += vRX;
						ballYVelocity[i] += vRY;
					} else {
						ballXVelocity[i] -= vRX * stringBounceEfficiency;
						ballYVelocity[i] -= vRY * stringBounceEfficiency;
					}
				}
			}
			
			if (mag < stringLength[i] * 0.98) {
				stringIsTight[i] = 0;
			}
			else {
				stringIsTight[i] = 1;
			}

		}
	}

	handleCollisions();

}
void BallSet::draw(HDC hdc, int W, int H) {

	HGDIOBJ oldObj = SelectObject(hdc, penLooseString);

	for (int i = 0; i < N; i++) {
		if (stringIsTight[i] == 0) {
			SelectObject(hdc, penLooseString);
		} else {
			SelectObject(hdc, penTightString);
		}

		if (stringLength[i] > 0.0) {
			MoveToEx(hdc, stringAnchorX[i], stringAnchorY[i], NULL);
			LineTo(hdc, ballX[i]+ballRadius[i], ballY[i]+ballRadius[i]);
		}
	}
	SelectObject(hdc, oldObj);
	///////  DRAW THE FILLED GREEN CIRCLE BITMAP

	for (int i = 0; i < N; i++) {
		/// These are all the individual masks and operations

		int D = ballIntDiameter[i];
		/// These next two are the real drawing
		StretchDIBits(hdc,
			(int)ballX[i], (int)ballY[i], D, D,
			0, 0, D, D,
			ballArrayMask[i], &ballBmpInfo[i], DIB_RGB_COLORS, SRCAND);
		StretchDIBits(hdc,
			(int)ballX[i], (int)ballY[i], D, D,
			0, 0, D, D,
			ballArray[i], &ballBmpInfo[i], DIB_RGB_COLORS, SRCPAINT);

	}

	updatePosition(W, H);
}
