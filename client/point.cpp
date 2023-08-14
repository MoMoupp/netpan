#include "point.h"

Point::Point(int x,int y)
{
    m_x = x;
    m_y = y;
}

void Point::setX(int x)
{
    m_x = x;
}

int Point::x()
{
    return m_x;
}

void Point::setY(int y)
{
    m_y = y;
}

int Point::y()
{
    return m_y;
}

void Point::setXY(int x, int y)
{
    m_x = x;
    m_y = y;
}
