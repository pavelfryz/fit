#version 330

in vec3 fPosition;
in vec3 fNormal;
in vec3 fColor;

/* Svetlo */
uniform vec3 lightPosition = vec3(100,500,100);
uniform vec3 La = vec3(0.6,0.6,0.6);
uniform vec3 Ld = vec3(0.4,0.4,0.4);

layout(location=0)out vec4 fragColor;

void main() {
    vec3 N = normalize(fNormal);
    vec3 L = normalize(lightPosition-fPosition);

    vec3 A = La*fColor;
    vec3 D = Ld*max(dot(L,N),0)*fColor;

    fragColor = vec4(A+D,1);
    //fragColor = vec4(fColor,1);
}
