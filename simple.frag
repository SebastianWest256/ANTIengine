#version 430 core
out vec4 FragColor;
uniform float time;

void main() {
    // Calculate distance from the center of the screen
    vec2 center = vec2(0.5, 0.5); // Assume screen coordinates are normalized (0.0 to 1.0)
    vec2 normCoord = gl_FragCoord.xy / vec2(800.0, 600.0); // Replace 800.0 and 600.0 with actual screen dimensions
    float distanceFromCenter = length(normCoord - center);

    // Create a red expanding wave based on the distance from the center and time
    float redWave = sin(distanceFromCenter * 100.0 - time * 2.0);

    // Normalize the wave to a 0.0 to 1.0 range
    float redIntensity = redWave * 0.5 + 0.5;

    // Set the color to the red intensity with no green or blue
    vec3 color = vec3(redIntensity, 0.0, 0.0);

    FragColor = vec4(color, 1.0);
}
