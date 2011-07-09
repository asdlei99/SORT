/*
 * filename :	merl.h
 *
 * programmer :	Cao Jiayin
 */

#ifndef	SORT_MERL
#define	SORT_MERL

// include the header file
#include "bxdf.h"

/////////////////////////////////////////////////////////////////////////////////////
//	definition of merl bxdf
//	desc :	Merl is short for Mitsubishi Electric Research Laboratories. They provide
//			some measured brdf on the websize http://www.merl.com/brdf/. Merl class is
//			responsible for loading the brdf file they provided.
class Merl : public Bxdf
{
// public method
public:
	// default constructor for clone
	Merl(){ m_data = 0; }
	// constructor from a filename
	// para 'filename' : the file name for the brdf
	Merl( const string& filename );
	// destructor
	~Merl();

	// whether the data is valid
	bool	IsValid() { return m_data != 0 ; }

	// evaluate bxdf
	// para 'wo' : out going direction
	// para 'wi' : in direction
	// result    : the portion that comes along 'wo' from 'wi'
	virtual Spectrum f( const Vector& wo , const Vector& wi ) const;

	// sample a direction randomly
	// para 'wo'  : out going direction
	// para 'wi'  : in direction generated randomly
	// para 'pdf' : property density function value of the specific 'wi'
	// result     : brdf value for the 'wo' and 'wi'
	virtual Spectrum Sample_f( const Vector& wo , Vector& wi , float* pdf ) const;

	// clone merl bxdf
	Merl*	Clone() const;

// private field
private:
	// the brdf data
	double*	m_data;

	// initialize default data
	void _init();
	// release data
	void _release();
	// load brdf data from file
	void _loadBrdf( const string& filename );
};

#endif