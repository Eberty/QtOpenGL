#version 130

uniform vec3 uLPos;
uniform vec3 uCamPos;

uniform vec4 matAmb;
uniform vec4 matDif;
uniform vec4 matSpec;

uniform sampler2D uTextureID;
uniform int uTexLoad;
uniform int uMaterial;

in vec3 vNormal;
in vec3 vPosW;
in vec2 vCoords;

void main(void) {
  vec4 vColor = (uTexLoad > 0) ? texture(uTextureID, vCoords) : matDif;

  vec4 ambient = vec4(vColor.rgb * matAmb.rgb, matAmb.a);
  vec3 vL = normalize(uLPos - vPosW);
  float cTeta = max(dot(vL, vNormal), 0.0);
  vec4 diffuse = vec4(vColor.rgb * matDif.rgb * cTeta, matDif.a);

  vec3 vV = normalize(uCamPos - vPosW);
  vec3 vR = normalize(reflect(-vL, vNormal));
  float cOmega = max(dot(vV, vR), 0.0);
  vec4 specular = vec4(vColor.rgb * matSpec.rgb * pow(cOmega, 20.0), matSpec.a);

  gl_FragColor = (uMaterial > 0) ? clamp(ambient + diffuse + specular, 0.0, 1.0) : vColor;
}
