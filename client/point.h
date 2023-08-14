#ifndef POINT_H
#define POINT_H


class Point
{
public:
    Point(int x=0,int y=0);

    void setX(int x);
    int x();

    void setY(int y);
    int y();

    void setXY(int x,int y);

private:
    int m_x;
    int m_y;
};

#endif // POINT_H
