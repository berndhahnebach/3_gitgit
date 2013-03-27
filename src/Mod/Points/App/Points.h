/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef POINTS_POINT_H
#define POINTS_POINT_H

#include <vector>
#include <iterator>

#include <Base/Vector3D.h>
#include <Base/Matrix.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include <App/PropertyStandard.h>
#include <App/PropertyGeo.h>

namespace Points
{


/** Point kernel
 */
class PointsExport PointKernel : public Data::ComplexGeoData
{
    TYPESYSTEM_HEADER();

public:
    PointKernel(void)
    {
    }
    PointKernel(unsigned long size)
    {
        resize(size);
    }
    virtual ~PointKernel()
    {
    }

    void operator = (const PointKernel&);

    /** @name Subelement management */
    //@{
    /** Sub type list
     *  List of different subelement types
     *  its NOT a list of the subelements itself
     */
    virtual std::vector<const char*> getElementTypes(void) const;
    virtual unsigned long countSubElements(const char* Type) const;
    /// get the subelement by type and number
    virtual Data::Segment* getSubElement(const char* Type, unsigned long) const;
    //@}

    inline void setTransform(const Base::Matrix4D& rclTrf){_Mtrx = rclTrf;}
    inline Base::Matrix4D getTransform(void) const{return _Mtrx;}
    std::vector<Base::Vector3d>& getBasicPoints()
    { return this->_Points; }
    const std::vector<Base::Vector3d>& getBasicPoints() const
    { return this->_Points; }
    void getFaces(std::vector<Base::Vector3d> &Points,std::vector<Facet> &Topo,
        float Accuracy, uint16_t flags=0) const;

    virtual void transformGeometry(const Base::Matrix4D &rclMat);
    virtual Base::BoundBox3d getBoundBox(void)const;

    /** @name I/O */
    //@{
    // Implemented from Persistence
    unsigned int getMemSize (void) const;
    void Save (Base::Writer &writer) const;
    void SaveDocFile (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);
    void RestoreDocFile(Base::Reader &reader, const int FileVersion);
    void save(const char* file) const;
    void save(std::ostream&) const;
    void load(const char* file);
    void load(std::istream&);
    //@}

private:
    Base::Matrix4D _Mtrx;
    std::vector<Base::Vector3d> _Points;

public:
    typedef std::vector<Base::Vector3d>::difference_type difference_type;
    typedef std::vector<Base::Vector3d>::size_type size_type;

    /// number of points stored 
    size_type size(void) const {return this->_Points.size();}
    void resize(unsigned int n){_Points.resize(n);}
    void reserve(unsigned int n){_Points.reserve(n);}
    inline void erase(unsigned long first, unsigned long last) {
        _Points.erase(_Points.begin()+first,_Points.begin()+last);
    }

    void clear(void){_Points.clear();}


    /// get the points
    inline const Base::Vector3d getPoint(const int idx) const {
        return transformToOutside3d(_Points[idx]);
    }
    /// set the points
    inline void setPoint(const int idx,const Base::Vector3d& point) {
        _Points[idx] = transformToInside3d(point);
    }
    /// insert the points
    inline void push_back(const Base::Vector3d& point) {
        _Points.push_back(transformToInside3d(point));
    }

    class PointsExport const_point_iterator
    {
    public:
        typedef std::vector<Base::Vector3d>::const_iterator iter_type;
        typedef iter_type::difference_type difference_type;
        typedef iter_type::iterator_category iterator_category;
        typedef const Base::Vector3d* pointer;
        typedef const Base::Vector3d& reference;
        typedef Base::Vector3d value_type;

        const_point_iterator(const PointKernel*, std::vector<Base::Vector3d>::const_iterator index);
        const_point_iterator(const const_point_iterator& pi);
        //~const_point_iterator();

        const_point_iterator& operator=(const const_point_iterator& fi);
        const Base::Vector3d& operator*();
        const Base::Vector3d* operator->();
        bool operator==(const const_point_iterator& fi) const;
        bool operator!=(const const_point_iterator& fi) const;
        const_point_iterator& operator++();
        const_point_iterator  operator++(int);
        const_point_iterator& operator--();
        const_point_iterator  operator--(int);
        const_point_iterator  operator+ (difference_type off) const;
        const_point_iterator  operator- (difference_type off) const;
        const_point_iterator& operator+=(difference_type off);
        const_point_iterator& operator-=(difference_type off);
        difference_type operator- (const const_point_iterator& right) const;
    private:
        void dereference();
        const PointKernel* _kernel;
        Base::Vector3d _point;
        std::vector<Base::Vector3d>::const_iterator _p_it;
    };

    typedef const_point_iterator const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    /** @name Iterator */
    //@{
    const_point_iterator begin() const
    { return const_point_iterator(this, _Points.begin()); }
    const_point_iterator end() const
    { return const_point_iterator(this, _Points.end()); }
    const_reverse_iterator rbegin() const
    { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const
    { return const_reverse_iterator(begin()); }
    //@}
};

} // namespace Points


#endif // POINTS_POINTPROPERTIES_H 
