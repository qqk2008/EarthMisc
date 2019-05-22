/*
* This source file is part of the osgOcean library
* 
* Copyright (C) 2009 Kim Bale
* Copyright (C) 2009 The University of Hull, UK
* 
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.

* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
* http://www.gnu.org/copyleft/lesser.txt.
*/

// ------------------------------------------------------------------------------
// -- THIS FILE HAS BEEN CREATED AS PART OF THE BUILD PROCESS -- DO NOT MODIFY --
// ------------------------------------------------------------------------------

static const char osgOcean_ocean_scene_frag[] =
	"// osgOcean Uniforms\n"
	"// -----------------\n"
	"uniform float osgOcean_DOF_Near;\n"
	"uniform float osgOcean_DOF_Focus;\n"
	"uniform float osgOcean_DOF_Far;\n"
	"uniform float osgOcean_DOF_Clamp;\n"
	"\n"
	"uniform float osgOcean_UnderwaterFogDensity;\n"
	"uniform float osgOcean_AboveWaterFogDensity;\n"
	"uniform vec4  osgOcean_UnderwaterFogColor;\n"
	"uniform vec4  osgOcean_AboveWaterFogColor;\n"
	"\n"
	"uniform float osgOcean_WaterHeight;\n"
	"\n"
	"uniform bool osgOcean_EnableGlare;\n"
	"uniform bool osgOcean_EnableDOF;\n"
	"uniform bool osgOcean_EyeUnderwater;\n"
	"uniform bool osgOcean_EnableUnderwaterScattering;\n"
	"// -------------------\n"
	"\n"
	"uniform sampler2D uTextureMap;\n"
	"\n"
	"varying vec3 vExtinction;\n"
	"varying vec3 vInScattering;\n"
	"varying vec3 vNormal;\n"
	"varying vec3 vLightDir;\n"
	"varying vec3 vEyeVec;\n"
	"\n"
	"varying float vWorldHeight;\n"
	"\n"
	"float computeDepthBlur(float depth, float focus, float near, float far, float clampval )\n"
	"{\n"
	"   float f;\n"
	"   if (depth < focus){\n"
	"      f = (depth - focus)/(focus - near);\n"
	"   }\n"
	"   else{\n"
	"      f = (depth - focus)/(far - focus);\n"
	"      f = clamp(f, 0.0, clampval);\n"
	"   }\n"
	"   return f * 0.5 + 0.5;\n"
	"}\n"
	"\n"
	"vec4 lighting( vec4 colormap )\n"
	"{\n"
	"	vec4 final_color = gl_LightSource[osgOcean_LightID].ambient * colormap;\n"
	"\n"
	"	vec3 N = normalize(vNormal);\n"
	"	vec3 L = normalize(vLightDir);\n"
	"\n"
	"	float lambertTerm = dot(N,L);\n"
	"\n"
	"	if(lambertTerm > 0.0)\n"
	"	{\n"
	"		final_color += gl_LightSource[osgOcean_LightID].diffuse * lambertTerm * colormap;\n"
	"\n"
	"		vec3 E = normalize(vEyeVec);\n"
	"		vec3 R = reflect(-L, N);\n"
	"\n"
	"		float specular = pow( max(dot(R, E), 0.0), 2.0 );\n"
	"\n"
	"		final_color += gl_LightSource[osgOcean_LightID].specular * specular;\n"
	"	}\n"
	"\n"
	"	return final_color;\n"
	"}\n"
	"\n"
	"float computeFogFactor( float density, float fogCoord )\n"
	"{\n"
	"	return exp2(density * fogCoord * fogCoord );\n"
	"}\n"
	"\n"
	"void main(void)\n"
	"{\n"
	"    vec4 final_color;\n"
	"\n"
	"    vec4 textureColor = texture2D( uTextureMap, gl_TexCoord[0].st );\n"
	"\n"
	"    // Underwater\n"
	"    // +2 tweak here as waves peak above average wave height,\n"
	"    // and surface fog becomes visible.\n"
	"    if(osgOcean_EyeUnderwater && vWorldHeight < osgOcean_WaterHeight+2.0 )\n"
	"    {\n"
	"        final_color = lighting( textureColor );\n"
	"\n"
	"        // mix in underwater light\n"
	"        if(osgOcean_EnableUnderwaterScattering)\n"
	"        {\n"
	"            final_color.rgb = final_color.rgb * vExtinction + vInScattering;\n"
	"        }\n"
	"\n"
	"        float fogFactor = computeFogFactor( osgOcean_UnderwaterFogDensity, gl_FogFragCoord );\n"
	"\n"
	"        // write to depth buffer (actually a GL_LUMINANCE)\n"
	"        if(osgOcean_EnableDOF)\n"
	"        {\n"
	"            float depth = computeDepthBlur(gl_FogFragCoord, osgOcean_DOF_Focus, osgOcean_DOF_Near, osgOcean_DOF_Far, osgOcean_DOF_Clamp);\n"
	"            gl_FragData[1] = vec4(depth);\n"
	"        }\n"
	"\n"
	"        // color buffer\n"
	"        gl_FragData[0] = mix( osgOcean_UnderwaterFogColor, final_color, fogFactor );\n"
	"    }\n"
	"    // Above water\n"
	"    else\n"
	"    {\n"
	"        final_color = lighting( textureColor );\n"
	"\n"
	"        float fogFactor = computeFogFactor( osgOcean_AboveWaterFogDensity, gl_FogFragCoord );\n"
	"        final_color = mix( osgOcean_AboveWaterFogColor, final_color, fogFactor );\n"
	"\n"
	"        // write to luminance buffer\n"
	"        // might not need the IF here, glsl compiler doesn't complain if \n"
	"        // you try and write to a FragData index that doesn't exist. But since\n"
	"        // Mac GLSL support seems so fussy I'll leave it in.\n"
	"        if(osgOcean_EnableGlare)\n"
	"        {\n"
	"            gl_FragData[1] = vec4(0.0);\n"
	"        }\n"
	"\n"
	"        // write to color buffer\n"
	"        gl_FragData[0] = final_color;\n"
	"    }\n"
	"}\n";
