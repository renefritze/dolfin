// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __ELEMENT_FUNCTION_H
#define __ELEMENT_FUNCTION_H

#include <dolfin/dolfin_log.h>
#include <dolfin/constants.h>
#include <dolfin/Point.h>
#include <dolfin/FunctionSpace.h>
#include <dolfin/Integral.h>

namespace dolfin {

  class FunctionSpace::ShapeFunction;
  class FunctionSpace::Product;
  class Function;
  
  class FunctionSpace::ElementFunction {
  public:

    // Empty constructor for v = 0
    ElementFunction();
    
    // Initialisation
    ElementFunction(real a);
    ElementFunction(const ShapeFunction   &v);
    ElementFunction(const Product         &v);
    ElementFunction(const ElementFunction &v);
    
    // Constructors for a * v
    ElementFunction(real a, const ShapeFunction   &v);
    ElementFunction(real a, const Product         &v);
    ElementFunction(real a, const ElementFunction &v);
    
    // Constructors for a0 * v0 + a1 * v1
    ElementFunction(real a0, const ShapeFunction   &v0, real a1, const ShapeFunction   &v1);
    ElementFunction(real a0, const Product         &v0, real a1, const Product         &v1);
    ElementFunction(real a0, const ElementFunction &v0, real a1, const ElementFunction &v1);
    ElementFunction(real a0, const ShapeFunction   &v0, real a1, const Product         &v1);
    ElementFunction(real a0, const ShapeFunction   &v0, real a1, const ElementFunction &v1);
    ElementFunction(real a0, const Product         &v0, real a1, const ElementFunction &v1);
    
    // Constructors for  v0 * v1
    ElementFunction(const ShapeFunction   &v0, const ShapeFunction   &v1);
    ElementFunction(const Product         &v0, const Product         &v1);
    ElementFunction(const ElementFunction &v0, const ElementFunction &v1);
    ElementFunction(const ShapeFunction   &v0, const Product         &v1);
    ElementFunction(const ShapeFunction   &v0, const ElementFunction &v1);
    ElementFunction(const Product         &v0, const ElementFunction &v1);
    
    // Constructor for scalar product
    ElementFunction(const ElementFunction& v0, const ElementFunction& v1, const ElementFunction& v2,
		    const ElementFunction& w0, const ElementFunction& w1, const ElementFunction& w2);
    
    // Destructor
    ~ElementFunction();
    
    //--- Operators ---
    
    // Evaluation
    real operator() (real x, real y, real z, real t) const;
    real operator() (Point p) const;
    
    // Assignment
    ElementFunction& operator= (real a);
    ElementFunction& operator= (const ShapeFunction   &v);
    ElementFunction& operator= (const Product         &v);
    ElementFunction& operator= (const ElementFunction &v);
    
    // Addition
    ElementFunction  operator+ (const ShapeFunction   &v) const;
    ElementFunction  operator+ (const Product         &v) const;
    ElementFunction  operator+ (const ElementFunction &v) const;
    ElementFunction  operator+=(const ShapeFunction   &v);
    ElementFunction  operator+=(const Product         &v);
    ElementFunction  operator+=(const ElementFunction &v);
    
    // Subtraction
    ElementFunction  operator- (const ShapeFunction   &v) const;
    ElementFunction  operator- (const Product         &v) const;
    ElementFunction  operator- (const ElementFunction &v) const;
    
    // Multiplication
    ElementFunction  operator* (real a)                   const;
    ElementFunction  operator* (const ShapeFunction   &v) const;
    ElementFunction  operator* (const Product         &v) const;
    ElementFunction  operator* (const ElementFunction &v) const;

    // Set the number of terms
    void init(int size);
    
    // Set values
    void set(int i, const ShapeFunction &v, real value);
   
    // Integration
    real operator* (Integral::Measure& dm) const;
    
    // Output
    friend LogStream& operator<<(LogStream& stream, const ElementFunction& v);
    
    // Friends
    friend class Function;

    // Vector element function
    class Vector {
    public:
      
      Vector(int size);
      Vector(const Vector& v);
      Vector(const ElementFunction& v0, const ElementFunction& v1, const ElementFunction& v2);
      ~Vector();
      
      int size() const;
      
      ElementFunction& operator() (int i);
      const ElementFunction operator, (const Vector& v) const;
      
    private:
      ElementFunction* v;
      int _size;
    };
    
  private:
    
    int n;      // Number of terms
    real *a;    // Coefficients
    real c;     // Constant (offset)
    Product *v; // Products
    
  };
  
  FunctionSpace::ElementFunction operator* (real a, const FunctionSpace::ElementFunction& v);
  
}

#endif
