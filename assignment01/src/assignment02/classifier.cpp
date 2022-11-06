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

void Classifier::classify(const std::vector<cv::Point2f>& path)
{
	cv::namedWindow("Drawing");
    // equidistant sampling
    simplify(path);
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

	int xScaler = 100 / abs(maxX - minX);
	int yScaler = 100 / abs(maxY - minY)*-1;	// -1 to mirror y

	for (int i = 0; i < m_simplifiedPath.size(); i++) {
		m_simplifiedPath[i].x -= minX;
		m_simplifiedPath[i].x *= xScaler;
		m_simplifiedPath[i].y -= minY;
		m_simplifiedPath[i].y *= yScaler;
	}

    // match using knn
	int k = 3;
	/*
	Vektor der von den k nächsten Nachbarn die Distanz zur gemalten Zahl und das label speichert. So kann der Vektor einfach mit sort() nach Distanz sortiert werden
	*/
	std::vector< pair <float, int>> nearestNeighbours(k)	
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
	for (int i = 0; i < c_dataSet.size(); i++; {
		distance = 0;	
		digit = c_dataSet[i];
		for(int j = 0; j<digit.points().size(); j++){ 
			distance += norm(digit.points(i)-m_simplifiedPath[i]);
		}
		if(nearestNeighbours.size() < k){
			nearestNeighbours.push_back((distance, digit.label()));
			nearestNeighbours.sort();
		} else if(nearestNeighbours[0].y > distance){
			nearestNeighbours[0] = (distance, digit.label());
			nearestNeighbours.sort();
		}
	}

	std::vector<int> labels(10); //Vektor in dem gespeichert wird welches Label wie oft in den k-nearest-neighbours vorkam

	for (int i = 0; i <k; i++){
		labels[nearestNeighbours[i].y]++;
	}
	
    m_result = std::distance(
		labels.begin(),std::max_element(labels.begin(), labels.end())); //gibt hoffentlich die position des größten elements im array wieder
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
	int point_intervall = path.size() / 8;
	for (int i = 0; i < path.size(); i += point_intervall) {
		m_simplifiedPath.push_back(path[i]);
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
