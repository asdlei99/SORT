/*
 * filename :	texture.cpp
 *
 * programmer : Cao Jiayin
 */

// include the header file
#include "texture.h"
#include <math.h>

// static value
TexShower* Texture::m_pTexShower = 0;

// default constructor
Texture::Texture()
{
	// inintialize default value
	_init();
}

// destructor
Texture::~Texture()
{
}

// initialize default value
void Texture::_init()
{
	m_pTexShower = 0;
	m_iTexWidth = 0;
	m_iTexHeight = 0;
	m_TexCoordFilter = TCF_WARP;
}

// set the texture shower
void Texture::SetTexShower( TexShower* texShower )
{
	// delete previous texture shower
	SAFE_DELETE( m_pTexShower );

	// set new one
	m_pTexShower = texShower;
}

// output the texture
bool Texture::Output( const string& str )
{
	if( m_pTexShower )
		return m_pTexShower->Output( this , str );
	
	cout<<"there is no texture shower"<<endl;

	return false;
}

// get the color
const Spectrum& Texture::GetColor( float u , float v ) const
{
	return GetColor( (int) (u *  m_iTexWidth) , (int) (v * m_iTexHeight) );
}

// set texture coordinate filter mode
void Texture::SetTexCoordFilter( TEXCOORDFILTER mode )
{
	m_TexCoordFilter = mode;
}

// do texture coordinate filter
void Texture::_texCoordFilter( int& x , int& y ) const
{
	switch( m_TexCoordFilter )
	{
	case TCF_WARP:
		if( x >= 0 ) 
			x = x % m_iTexWidth;
		else
			x = m_iTexWidth - ( -x ) % m_iTexWidth;
		if( y >= 0 )
			y = y % m_iTexHeight;
		else
			y = m_iTexHeight - ( -y ) % m_iTexHeight;
		break;
	case TCF_CLAMP:
		x = min( (int)m_iTexWidth - 1 , max( x , 0 ) );
		y = min( (int)m_iTexHeight - 1 , max( y , 0 ) );
		break;
	case TCF_MIRROR:
		x = ( x >= 0 )?x:(1-x);
		x = x % ( 2 * m_iTexWidth );
		x -= m_iTexWidth;
		x = ( x >= 0 )?x:(1-x);
		x = m_iTexWidth - 1 - x;
		y = ( y >= 0 )?y:(1-y);
		y = y % ( 2 * m_iTexHeight );
		y -= m_iTexHeight;
		y = ( y >= 0 )?y:(1-y);
		y = m_iTexHeight - 1 - y;
		break;
	}
}
