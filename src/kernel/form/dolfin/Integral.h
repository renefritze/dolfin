// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __INTEGRAL_H
#define __INTEGRAL_H

#include <dolfin/constants.h>
#include <dolfin/Tensor.h>
#include <dolfin/FunctionSpace.h>

namespace dolfin {

  class Quadrature;
  class Map;
  
  class Integral {
  public:

    // Forward declarations of nested classes
    class Measure;
    class InterioriMeasure;
    class BoundaryMeasure;
    
    /// Integral measure (base class)
    class Measure
    {
    public:
      
      Measure();
      Measure(const Map& map, const Quadrature& quadrature);
      virtual ~Measure();
      
      // Update map and quadrature
      virtual void update(const Map& map, const Quadrature& quadrature);
      
      // Integration operators dm * v
      real operator* (real a) const;
      real operator* (const FunctionSpace::ShapeFunction& v);
      real operator* (const FunctionSpace::Product& v);
      real operator* (const FunctionSpace::ElementFunction& v);
      
    protected:
      
      // Evaluation of integrals
      virtual real integral(const FunctionSpace::ShapeFunction& v) = 0;
      virtual real integral(const FunctionSpace::Product& v) = 0;
      virtual real det() const = 0;
      
      // Init table
      void init();
      
      // Resize table
      void resize(int new_order, int new_n);
      
      // Integral data
      class Value {
      public:
	
	Value() {
	  value = 0.0;
	  computed = false;
	}
	
	// Evaluation
	real operator() () {
	  return value;
	}
	
	// Check if value has been set
	bool ok() {
	  return computed;
	}
	
	// Set value
	void set(real value) {
	  this->value = value;
	  computed = true;
	}
	
	// Initialisation to zero, argument a is ignored
	void operator= (int a) {
	  value = 0.0;
	  computed = false;
	}
	
      private:
	
	real value;
	bool computed;
	
      };
      
      const Map* m;     // Map from reference cell
      const Quadrature* q;  // Quadrature rule on reference cell
      
      int order;            // Maximum number of factors
      int n;                // Number of different shape functions
      Tensor<Value>* table; // A lookup table for integrals
      bool active;          // True if the measure is active
      
    };
    
    // Integral measure for the interior of an element
    class InteriorMeasure : public Measure
    {
    public:
      
      /// Constructor
      InteriorMeasure();

      /// Constructor
      InteriorMeasure(Map& m, Quadrature& q);
      
      // Update map and quadrature
      void update(const Map& map, const Quadrature& quadrature);

    private:
      
      // Evaluate integral of given shape function
      real integral(const FunctionSpace::ShapeFunction& v);

      // Evaluate integral of given product
      real integral(const FunctionSpace::Product& v);
      
      // Return determinant of map
      real det() const;
      
    };
    
    // Integral measure for the boundary of an element
    class BoundaryMeasure : public Measure
    {
    public:
      
      // Constructor
      BoundaryMeasure();

      // Constructor
      BoundaryMeasure(Map& m, Quadrature& q);
      
      // Update map and quadrature
      void update(const Map& map, const Quadrature& quadrature);

    private:
      
      // Evaluate integral of given shape function
      real integral(const FunctionSpace::ShapeFunction& v);

      // Evaluate integral of given product
      real integral(const FunctionSpace::Product& v);
      
      // Return determinant of map
      real det() const;

      // Current boundary
      int boundary;

    };
    
  };
  
  real operator* (real a, const Integral::Measure& dm);
  
}

#endif
