#include "BezierPath.hpp"
#include <stack>

BezierPath::BezierPath() {
}

BezierPath::~BezierPath() {
}

void BezierPath::setControlPoints( const std::vector<cc::Vec3f>& newControlPoints ) {
	_controlPoints = newControlPoints;
	_curveCount = (static_cast<unsigned int>(_controlPoints.size())-1)/3;
}

const std::vector<cc::Vec3f> BezierPath::getControlPoints() const {
	return _controlPoints;
}

void BezierPath::interpolate( const std::vector<cc::Vec3f>& segmentPoints, float scale ) {
	_controlPoints.clear();

	if( segmentPoints.size() < 2 ) {
		return;
	}

	for( size_t i = 0; i < segmentPoints.size(); ++i ) {
		if( 0 == i ) { // first
			const cc::Vec3f& p1 = segmentPoints[i];
			const cc::Vec3f& p2 = segmentPoints[i+1];
			const cc::Vec3f tangent = (p2 - p1);
			const cc::Vec3f q1 = p1 + scale * tangent;
			_controlPoints.push_back(p1);
			_controlPoints.push_back(q1);
		} else if( segmentPoints.size()-1 == i ) { // last
			const cc::Vec3f& p0 = segmentPoints[i-1];
			const cc::Vec3f& p1 = segmentPoints[i];
			const cc::Vec3f tangent = (p1 - p0);
			const cc::Vec3f q0 = p1 - scale * tangent;
			_controlPoints.push_back(q0);
			_controlPoints.push_back(p1);
		} else {
			const cc::Vec3f& p0 = segmentPoints[i-1];
			const cc::Vec3f& p1 = segmentPoints[i];
			const cc::Vec3f& p2 = segmentPoints[i+1];
			const cc::Vec3f tangent = (p2 - p0).normalized();
			const cc::Vec3f q0 = p1 - scale * tangent * (p1 - p0).magnitude();
			const cc::Vec3f q1 = p1 + scale * tangent * (p2 - p1).magnitude();
			_controlPoints.push_back(q0);
			_controlPoints.push_back(p1);
			_controlPoints.push_back(q1);
		}
	}

	_curveCount = (static_cast<unsigned int>(_controlPoints.size())-1)/3;
}

void BezierPath::samplePoints( const std::vector<cc::Vec3f>& sourcePoints, float minSqrDist, float maxSqrDist, float scale ) {
	if( sourcePoints.size() < 2 ) {
		return;
	}

	std::stack<cc::Vec3f> samplePoints;
	samplePoints.push(sourcePoints[0]);
	cc::Vec3f potentialSamplePoint = sourcePoints[1];
	for( size_t i = 2; i < sourcePoints.size(); ++i ) {
		if( ((potentialSamplePoint - sourcePoints[i]).sqrMagnitude() > minSqrDist) &&
			  ((samplePoints.top() - sourcePoints[i]).sqrMagnitude() > maxSqrDist) ) {
			samplePoints.push(potentialSamplePoint);
		}
		potentialSamplePoint = sourcePoints[i];
	}
	// handle last bit of curve
	cc::Vec3f p1 = samplePoints.top(); // last sample point
	samplePoints.pop();
	const cc::Vec3f& p0 = samplePoints.top(); // second last sample point
	const cc::Vec3f tangent = (p0 - potentialSamplePoint).normalized();
	const float d2 = (potentialSamplePoint - p1).magnitude();
	const float d1 = (p1 - p0).magnitude();
	p1 = p1 + tangent * ((d1 - d2)/2.0f);
	samplePoints.push(p1);
	samplePoints.push(potentialSamplePoint);

	std::vector<cc::Vec3f> samplePointsVec;
	while( !samplePoints.empty() ) {
		samplePointsVec.push_back(samplePoints.top());
		samplePoints.pop();
	}
	interpolate(samplePointsVec, scale);
}

cc::Vec3f BezierPath::calculateBezierPoint( int curveIndex, float t ) {
	const int nodeIndex = curveIndex * 3;
	const cc::Vec3f& p0 = _controlPoints[nodeIndex];
	const cc::Vec3f& p1 = _controlPoints[nodeIndex+1];
	const cc::Vec3f& p2 = _controlPoints[nodeIndex+2];
	const cc::Vec3f& p3 = _controlPoints[nodeIndex+3];
	return calculateBezierPoint(t, p0, p1, p2, p3);
}

std::vector<cc::Vec3f> BezierPath::getDrawingPoints( unsigned int samples ) {
	std::vector<cc::Vec3f> drawingPoints;
	for( unsigned int curveIndex = 0; curveIndex < _curveCount; ++curveIndex ) {
		// only do this for the first endpoint.  when i!=0 this coincides with the endpoint of the previous segment
		if( 0 == curveIndex ) {
			drawingPoints.push_back(calculateBezierPoint(curveIndex, 0));
		}

		for( unsigned int j = 1; j <= samples; ++j ) {
			const float t = static_cast<float>(j) / static_cast<float>(samples);
			drawingPoints.push_back(calculateBezierPoint(curveIndex, t));
		}
	}
	return drawingPoints;
}

std::vector<cc::Vec3f> BezierPath::getDrawingPointsAlternate( unsigned int samples ) {
	std::vector<cc::Vec3f> drawingPoints;
	for( unsigned int i = 0; i < _controlPoints.size()-3; i += 3 ) {
		const cc::Vec3f& p0 = _controlPoints[0];
		const cc::Vec3f& p1 = _controlPoints[0+1];
		const cc::Vec3f& p2 = _controlPoints[0+2];
		const cc::Vec3f& p3 = _controlPoints[0+3];
		if( 0 == i ) {
			drawingPoints.push_back(calculateBezierPoint(0, p0, p1, p2, p3));
		}
		for( unsigned int j = 1; j <= samples; ++j ) {
			const float t = static_cast<float>(j) / static_cast<float>(samples);
			drawingPoints.push_back(calculateBezierPoint(t, p0, p1, p2, p3));
		}
	}
	return drawingPoints;
}

std::vector<cc::Vec3f> BezierPath::getDrawingPointsRecursive( unsigned int samples ) {
	std::vector<cc::Vec3f> drawingPoints;
	for( unsigned int curveIndex = 0; curveIndex < _curveCount; ++curveIndex ) {
		std::vector<cc::Vec3f> bezierCurveDrawingPoints = findDrawingPoints(curveIndex);
		if( curveIndex != 0 ) {
			// remove first point, as it coincides with the lsat point of the previous bezier curve
			bezierCurveDrawingPoints.erase(bezierCurveDrawingPoints.begin() + 0);
		}
		drawingPoints.insert(std::end(drawingPoints), std::begin(bezierCurveDrawingPoints), std::end(bezierCurveDrawingPoints));
	}
	return drawingPoints;
}

cc::Vec3f BezierPath::calculateBezierPoint( float t, const cc::Vec3f& p0, const cc::Vec3f& p1, const cc::Vec3f& p2, const cc::Vec3f& p3 ) {
	const float u = 1.0f - t;
	const float tt = t * t;
	const float uu = u * u;
	const float uuu = uu * u;
	const float ttt = tt * t;

	cc::Vec3f p = uuu * p0; // first term
	p += 3.0f * uu * t * p1; // second term
	p += 3.0f * u * tt * p2; // third term
	p += ttt * p3; // fourth term
	return p;
}

std::vector<cc::Vec3f> BezierPath::findDrawingPoints( unsigned int curveIndex ) {
	std::vector<cc::Vec3f> pointList;
	const cc::Vec3f left = calculateBezierPoint(curveIndex, 0);
	const cc::Vec3f right = calculateBezierPoint(curveIndex, 1);
	pointList.push_back(left);
	pointList.push_back(right);
	findDrawingPoints(curveIndex, 0, 1, pointList, 1);
	return pointList;
}

int BezierPath::findDrawingPoints( unsigned int curveIndex, float t0, float t1, std::vector<cc::Vec3f>& pointList, unsigned int insertionIndex ) {
	const float MIN_SQR_DIST = 0.01f;
	const float DIV_THRESHOLD = -0.99f; // 172 degrees, 8 degrees from straight line
	const cc::Vec3f left = calculateBezierPoint(curveIndex, t0);
	const cc::Vec3f right = calculateBezierPoint(curveIndex, t1);
	if( (left - right).sqrMagnitude() < MIN_SQR_DIST ) {
		return 0;
	}

	const float tMid = (t0 + t1) / 2.0f;
	const cc::Vec3f mid = calculateBezierPoint(curveIndex, tMid);

	const cc::Vec3f leftDirection = (left - mid).normalized();
	const cc::Vec3f rightDirection = (right - mid).normalized();

	if( leftDirection.dot(rightDirection) > DIV_THRESHOLD || fabsf(tMid - 0.5f) < 0.0001f ) {
		int pointsAddedCount = 0;
		pointsAddedCount += findDrawingPoints(curveIndex, t0, tMid, pointList, insertionIndex);
		pointList.insert(std::begin(pointList) + (insertionIndex + pointsAddedCount), mid);
		pointsAddedCount++;
		pointsAddedCount += findDrawingPoints(curveIndex, tMid, t1, pointList, insertionIndex + pointsAddedCount);
		return pointsAddedCount;
	}
	return 0;
}