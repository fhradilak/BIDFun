#include "classifier.hpp"
#include <utility>
#include <vector>
#include <algorithm>


Classifier::Classifier(std::vector<Digit> dataSet, const int k)
: c_dataSet(std::move(dataSet))
, c_simplifiedSize(c_dataSet.front().points().size())
, c_k(k)
{
}

bool sortNeighbours(std::pair<float, int> i, std::pair<float, int> j) { return (i.first < j.first); }
class CompareDist
{
public:
	bool operator()(std::pair<float, int> n1, std::pair<float, int> n2) {
		return n1.first > n2.first;
	}
};

void Classifier::classify(const std::vector<cv::Point2f>& path)
{
	cv::Mat newFrame = cv::Mat(1000, 1000, CV_64F,0.0);
	cv::Scalar color = cv::Scalar(100, 0, 50);
	cv::Mat secondFrame = cv::Mat(300, 300, CV_64F, 0.0);
	cv::Mat thirdFrame = cv::Mat(300, 300, CV_64F, 0.0);


	/*cv::namedWindow("Drawing");
	cv::namedWindow("Before");
	cv::namedWindow("After");
	*/
    // equidistant sampling
    simplify(path);

	for (int i = 0; i < m_simplifiedPath.size(); i++) {
		cv::circle(newFrame, m_simplifiedPath[i], 10, color, 5);
	}
	//cv::imshow("Before", newFrame);
	//m_simplifiedPath.erase(m_simplifiedPath.begin(), m_simplifiedPath.begin() + 8);
	m_simplifiedPath.resize(8); //Für knn darf der Vektor nur 8 Elemente haben

	

	float minX = m_simplifiedPath[0].x;
	float maxY = m_simplifiedPath[0].x;
	float minY= m_simplifiedPath[0].x;
	float maxX = m_simplifiedPath[0].x;

	for (int i = 0; i < m_simplifiedPath.size(); i++) {
		if (m_simplifiedPath[i].x < minX)
			minX = m_simplifiedPath[i].x;
		if (m_simplifiedPath[i].y < minY)
			minY = m_simplifiedPath[i].y;
		if (m_simplifiedPath[i].x > maxX)
			maxX = m_simplifiedPath[i].x;
		if (m_simplifiedPath[i].y > maxY)
			maxY = m_simplifiedPath[i].y;
	}

	/*
	wir haben das falsch verstanden und sollen nicht mit einem Bild, sondern doch nur mit den Punkten arbeiten. Deswegen hier die schicke per-Hand-skalierung (natürlich noch nicht getestet alles)
	*/


	float xScaler = 100 / abs(maxX - minX);
	float yScaler = 100 / abs(maxY - minY)*-1;	// -1 to mirror y


	for (int i = 0; i < m_simplifiedPath.size(); i++) {
		m_simplifiedPath[i].x -= minX;
		m_simplifiedPath[i].x *= xScaler;
		m_simplifiedPath[i].y -= minY;
		m_simplifiedPath[i].y *= yScaler;
		m_simplifiedPath[i].y += 100.0;

	}
	for (int i = 0; i < m_simplifiedPath.size(); i++) {
		cv::circle(secondFrame, m_simplifiedPath[i]+cv::Point2f(100,100), 10, color, 5);
		if(i<m_simplifiedPath.size()-1){
			cv::line(secondFrame, m_simplifiedPath[i] + cv::Point2f(100, 100), m_simplifiedPath[i + 1] + cv::Point2f(100, 100), color, 10, 5);
		}
		
	}
	//cv::imshow("After", secondFrame);

	Digit d = c_dataSet[1];
	//std::cout << d.label();
	for (int i = 0; i < d.points().size(); i++) {
		cv::circle(thirdFrame, d.points()[i] + cv::Point(100, 100), 10, color, 5);
	}
	//cv::imshow("Test", thirdFrame);

    // match using knn
	int k = 15;
	/*
	Vektor der von den k nächsten Nachbarn die Distanz zur gemalten Zahl und das label speichert. So kann der Vektor einfach mit sort() nach Distanz sortiert werden
	*/
	//F: Label is a Digit and not an int
	std::vector<std::pair<float, int>> nearestNeighbours(k);
	//F: prioqueue
	std::priority_queue< std::pair<float, int>, std::vector<std::pair<float, int>>, CompareDist> nN;
	float distance;
	/*
	naiver algo, da ich davon ausgehe, dass an der selben stelle angefangen wird die Zahl zu malen und unsere punkte ja auch nicht equally spaced sind:

	- gehe jede Zahl im Datensatz durch
	- berechne für jeden Punkt der Zahl die Distanz zum dem Punkt unsere og-Zahl der den selben Index hat
	- die Summe dieser Distanzen ist unser Abstandsmaß für zwei Zahlen
	- solange noch nicht k Zahlen im nearestNeigbour Vector sind pack einfach jede Zahl rein
	- sind schon Zahlen drin guck dir die Vorderste an. Da der Vektor sortiert ist hat diese Zahl die höchste Distanz. Wenn wir eine geringere Distanz haben tauschen wir die Einträge im Vektor aus
	- dann sortieren wir den Vektor wieder und machen weiter
	*/
	for (int i = 0; i < k; i++) {
		nearestNeighbours[i] = std::make_pair(100000000000.0, 0);
		
	}
	for (int i = 0; i < c_dataSet.size(); i++) {
		distance = 0;	
		Digit digit = c_dataSet[i];
		for(int j = 0; j<digit.points().size(); j++){ 
			cv::Point2f test= m_simplifiedPath[j];
			cv::Point2f test_F = cv::Point2f(digit.points()[j]);
			std::vector<cv::Point2f> contours2f;

			//std::transform(digit.points().begin(), digit.points().end(), std::back_inserter(contours2f), [](const cv::Point& p) {return (cv::Point2f)p; });


 			distance += cv::norm(cv::Point2f(digit.points()[j]) - m_simplifiedPath[j]);
		}
		
		if(nearestNeighbours.size() < k){
			nearestNeighbours.push_back((std::make_pair(distance, digit.label())));
			
			std::sort(nearestNeighbours.begin(), nearestNeighbours.end(), [](auto &left, auto &right) {
				return left.first > right.first;
			});
		} else if(nearestNeighbours[0].first > distance){
			nearestNeighbours[0] = std::make_pair(distance, digit.label());

			std::sort(nearestNeighbours.begin(), nearestNeighbours.end(), [](auto &left, auto &right) {
				return left.first > right.first;
			});
		}
		
	}

	std::vector<int> labels(10); //Vektor in dem gespeichert wird welches Label wie oft in den k-nearest-neighbours vorkam
	for (int i = 0; i < k; i++) {
		int t = nearestNeighbours[i].second;
		labels[t]++;
	}
	

	m_simplifiedPath.clear();
    m_result = std::distance(
		labels.begin(),std::max_element(labels.begin(), labels.end())); //gibt hoffentlich die position des größten elements im array wieder
	std::cout << m_result;
}

int Classifier::getResult() const
{
    return m_result;
}

std::vector<cv::Point2f> Classifier::getSimplifiedPath() const
{
    return m_simplifiedPath;
}

void Classifier::simplify(std::vector<cv::Point2f> path)
{
	float pathLength = 0;
	for (int i = 0; i < path.size() - 1; i++) {
		pathLength += cv::norm(path[i + 1] - path[i]);
	}

	int pointCount = 8;
	float segmentlength = pathLength / pointCount;
	float currentPosition = 0;
	float nextPointPosition = 0;
	int cpi = 0;

	while (currentPosition < pathLength)
	{
		while (nextPointPosition < currentPosition) {
			cpi++;
			nextPointPosition += cv::norm(path[cpi] - path[cpi - 1]);
		}
		m_simplifiedPath.push_back(path[cpi]);
		currentPosition += segmentlength;
	}






	/*	for equal spacing (not working):
	for (int i = 0; i < path.size(); i++) {
		pathLength += cv::norm(path[i + 1] - path[i]);
	}

	int pointCount = 8;
	//std::vector<cv::Point2f> path;
	std::vector<cv::Point2f> newPath;

	float segmentlength = pathLength / pointCount;
	float currentPosition = 0;
	float nextPointPosition = 0;
	float prevPointPosition = 0;
	int cpi = 0;

	while (pathLength >= currentPosition)
	{
		if (nextPointPosition == currentPosition) {
			newPath.push_back(path[cpi]);
			currentPosition += segmentlength;
			continue;
		}
		else if (nextPointPosition < currentPosition) {
			prevPointPosition = nextPointPosition;
			nextPointPosition += cv::norm(path[cpi + 1] - path[cpi]);
			cpi++;
			continue;
		}
		float distFromPrevPoint = currentPosition - prevPointPosition;
		cv::Point2f normVector = path[cpi] - path[cpi - 1];
		normVector = normVector / cv::norm(normVector);
		newPath.push_back(normVector * distFromPrevPoint + path[cpi - 1]);
		currentPosition += segmentlength;

		cv::Scalar color = cv::Scalar(100, 0, 50);
		cv::circle(colorFrame, newPath.back(), 10, color, 5);
	}*/
}
