// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __SHAPE_FUNCTION_H
#define __SHAPE_FUNCTION_H

#include <dolfin/dolfin_log.h>
#include <dolfin/constants.h>
#include <dolfin/FunctionPointer.h>
#include <dolfin/Point.h>
#include <dolfin/FunctionSpace.h>
#include <dolfin/Integral.h>

namespace dolfin {

  class FunctionSpace::Product;
  class FunctionSpace::ElementFunction;
  
  class FunctionSpace::ShapeFunction {
  public:
    
    // Empty constructor for v = 0
    ShapeFunction();
    
    // Constructor for v = 1
    ShapeFunction(int id, int component);
    
    // Initialisation
    ShapeFunction(function f);
    ShapeFunction(const ShapeFunction &v);
    
    // Specification of derivatives
    void set(ElementFunction ddX, ElementFunction ddY, ElementFunction ddZ, ElementFunction ddT);
    
    // Get id
    int id() const;

    // Get component number
    int component() const;
    
    // True if equal to zero
    bool zero() const;
    
    // True if equal to unity
    bool one() const;
    
    // Derivatives
    const ElementFunction& ddx() const;
    const ElementFunction& ddy() const;
    const ElementFunction& ddz() const;
    const ElementFunction& ddt() const;
    
    const ElementFunction& ddX() const;
    const ElementFunction& ddY() const;
    const ElementFunction& ddZ() const;
    const ElementFunction& ddT() const;
    
    // Update
    void update(const Map& map);
    
    //--- Operators ---
    
    // Evaluation
    real operator() (real x, real y, real z, real t) const;
    real operator() (Point p) const;
    
    // Assignment
    ShapeFunction& operator= (const ShapeFunction &v);
    
    // Addition
    ElementFunction operator+ (const ShapeFunction   &v) const;
    ElementFunction operator+ (const Product         &v) const;
    ElementFunction operator+ (const ElementFunction &v) const;
    
    // Subtraction
    ElementFunction operator- (const ShapeFunction   &v) const;
    ElementFunction operator- (const Product         &v) const;
    ElementFunction operator- (const ElementFunction &v) const;
    
    // Multiplication
    ElementFunction operator* (const real a)             const;
    Product         operator* (const ShapeFunction   &v) const;
    Product         operator* (const Product         &v) const;
    ElementFunction operator* (const ElementFunction &v) const;
    
    // Integration
    real operator* (Integral::Measure &dm) const;
    
    // Needed for ShortList
    void operator= (int zero);
    bool operator! () const;
    
    // Output
    friend LogStream& operator<<(LogStream& stream, const ShapeFunction &v);
    
    // Vector shape function
    class Vector {
    public:
      
      Vector(int size);
      Vector(const Vector& v);
      //Vector(const ShapeFunction& v0, const ShapeFunction& v1, const ShapeFunction& v2);
      ~Vector();
      
      int size() const;
      
      ShapeFunction& operator() (int i);
      
    private:
      ShapeFunction* v;
      int _size;
    };


  private:
    
    int _id;
    int _component;
    
  };
  
  FunctionSpace::ElementFunction operator* (real a, const FunctionSpace::ShapeFunction &v);
  
}

#endif
