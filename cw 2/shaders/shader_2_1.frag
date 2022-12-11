#version 430 core

precision mediump float;
flat in vec4 color;
flat in float out_time;

in vec4 pos_local; 
in vec4 pos_global;

out vec4 out_color;


void main()
{
	//out_color = pos_global;
	float x = floor( 10.0 * (pos_local.x + mod(out_time*0.3f, 1.0) ) );
	float y = floor( 10.0 * pos_local.y );
	if( mod( x, 2.0 ) == 0.0 && mod( y, 2.0 ) == 0.0 )
	{
		out_color = pos_local;
	}
	else if( mod( x, 2.0 ) == 0.0 || mod( y, 2.0 ) == 0.0 )
	{
		out_color = color;
	}
	else
	{
		out_color = pos_local;
	}
}