#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <opencv2/opencv.hpp>
#define N 100
#define radius 8

struct Point {
    double x = 0;
    double y = 0;
    Point(double begin, double end): x(begin), y(end){}
    Point() = default;
};

struct Vector {
    double b_x = 0;
    double b_y = 0;
    double v_x = 0;
    double v_y = 0;

    Vector(Point point1, Point point2): b_x(point1.x), b_y(point1.y), v_x(point2.x - point1.x), v_y(point2.y - point1.y){}
    Vector() = default;
};

double angle_between(Vector v1, Vector v2) {
    return acos((v1.v_x * v2.v_x + v1.v_y * v2.v_y) / ((sqrt(v1.v_x * v1.v_x + v1.v_y * v1.v_y)) * (sqrt(v2.v_x * v2.v_x + v2.v_y * v2.v_y))));
}

bool make_angle(Vector v1, Vector v2) {
    Vector delta(Point(v1.b_x, v1.b_y), Point(v2.b_x - v1.b_x, v2.b_y - v1.b_y));
    return angle_between(delta, v1) >= 0.1;
}

Point check_area(double x, double y, char area[radius][radius], char** bm) {
    Point left_up(0, N - radius);
    long long bias_y, bias_x;
    if (y < radius / 2) {
        bias_y = 0;
    } else if (y > N - radius / 2) {
        bias_y = y - radius / 2;
    } else {
        bias_y = N - radius;
    }

    if (x < radius / 2) {
        bias_x = 0;
    } else if (x > N - radius / 2)
    {
        bias_x = x - radius / 2;
    } else {
        bias_x = N - radius;
    }


    for (int i = 0; i < radius; i++)
    {
        for (int j = 0; j < radius; j++)
        {
            area[i][j] = bm[i + bias_x][j + bias_y];
        }
    }
    left_up.x = bias_x;
    left_up.y = bias_y;
    return left_up;
}

Point intersector(Vector v1, Vector v2) {
    Point ans(0, 0);
    Point p11(v1.b_x, v1.b_y), p12(v1.v_x + p11.x, v1.v_y + p11.y), p21(v2.b_x, v2.b_y), p22(v2.v_x + p21.x, v2.v_y + p21.y);

    if (((p11.y - p12.y) / (p21.y - p22.y)) == ((p12.x - p11.x) / (p22.x - p21.x)))
    {
        ans.x = -1;
        ans.y = -1;
        return ans;
    }

    double det = (p11.y - p12.y) * (p22.x - p21.x) - (p21.y - p22.y) * (p12.x - p11.x);
    ans.y = round(((p21.y - p22.y) * (p11.x * p12.y - p12.x * p11.y) - (p11.y - p12.y) * (p21.x * p22.y - p22.x * p21.y)) / det);
    ans.x = round(((p12.x - p11.x) * (p21.x * p22.y - p22.x * p21.y) - (p22.x - p21.x) * (p11.x * p12.y - p12.x * p11.y)) / det);
    return ans;
}

bool is_interrupt(const char* line) {
    bool black = false;
    bool start = true;
    int num_of_changes = 0;
    for (int i = 0; i < radius; i++) {
        if (start && line[i] == 0) {
            continue;
        }
        start = false;
        if (!black && line[i] == 1) {
            black = true;
        }
        if (black && line[i] == 0) {
            black = false;
        }
        if ((black && line[i] == 0) || (!black && line[i] == 1)) {
            num_of_changes++;
        }
    }
    if (num_of_changes > 1) {
        return true;
    }
    return false;
}

bool hasCollision (char area[radius][radius]) {
    char line[radius];
    bool hasCollision1 = false, hasCollision2 = false;
    for (int j = 0; j < radius && !hasCollision1; j++) {
        for (int i = 0 ; i < radius; i++) {
            line[i] = area[i][j];
        }
        if (is_interrupt(line)) {
            hasCollision1 = true;
            break;
        }
    }
    for (int i = 0; i < radius && !hasCollision2; i++) {
        for (int j = 0 ; j < radius; j++) {
            line[j] = area[i][j];
        }
        if (is_interrupt(line)) {
            hasCollision2 = true;
            break;
        }
    }
    return hasCollision1 && hasCollision2;
}

int main() {
    auto picture = cv::imread("test.jpg", CV_LOAD_IMAGE_COLOR);
    int n = picture.cols;
    int m = picture.rows;
    char area[radius][radius];
    std::vector<Vector> lines;
    int curr_vector_size;
    long long x,y;
    y = 0;
    while(y != m) {
        for (x = 0; x < n; x++) {
            if (picture[x][y] == 1) {
                Point left_up = check_area(x, y, area, picture);
                Point point1, point2;
                int found = 0;
                for (int i = 0; i < radius && found == 0 ; i++) {
                    for (int j = 0; j < radius - 3; j++) {
                        if (area[i][j] == 1 && area[i][j + 1] == 1 && area[i][j + 2] == 1) {
                            point1.x = j + left_up.x;
                            point1.y = i + left_up.y;
                        }
                        found = 1;
                        break;
                    }
                }
                found = 0;
                for (int i = radius; i > 0 && found == 0 ; i--) {
                    for (int j = 0; j < radius - 3; j++) {
                        if (area[i][j] == 1 && area[i][j + 1] == 1 && area[i][j + 2] == 1) {
                            point2.x = j + left_up.x;
                            point2.y = i + left_up.y;
                        }
                        found = 1;
                        break;
                    }
                }

                if (point1.x == point2.x && point1.y == point2.y) {
                    fprintf(stderr, "error of seeker!\n");
                    exit(-1);
                }

                Vector new_vector(point1, point2);
                curr_vector_size = lines.size();
                bool isCorrect = true;
                for (int i = 0; i < curr_vector_size; i++) {
                    isCorrect = make_angle(lines[i], new_vector);
                }
                if (isCorrect) {
                    lines.push_back(new_vector);
                }
            }
        }
        y++;
    }
    int answer = 0;
    for (int i = 0; i < curr_vector_size - 1; i++) {
        for (int j = i + 1; j < curr_vector_size; j++) {
            Point intersection = intersector(lines[i], lines[j]);
            if ( (intersection.x > n) || (intersection.y > m) || (intersection.x < 0) || (intersection.y < 0)) {
                continue;
            }
            else if (hasCollision(area)) {
                    answer++;
            }
        }
    }
    return answer;
}