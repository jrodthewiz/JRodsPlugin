/*
  ==============================================================================

    OpenGLComponent.h
    Created: 25 Jan 2023 11:54:17pm
    Author:  wolfk

    https://www.shadertoy.com/view/XstBR8

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


using namespace ::juce::gl;

class OpenGLComponent : public juce::Component,
                        public juce::OpenGLRenderer
{
public:
    OpenGLComponent();
    ~OpenGLComponent() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;
    void setMousePos(juce::Point<int> point);

    void paint(juce::Graphics& g) override;

    float DialValue;
    float DialValue2;
    float DialValue3;

    float time;

    float AudioData;

    //FFT split into 3 buckets
    float TimeStamp;
    float mid;
    float high;

    float maxIterations = 10;


    juce::Point<int> mousePos = juce::Point<int>(7, 25);


private:
    
    juce::OpenGLContext openGLContext;

    struct Vertex
    {
        float position[2];
        float colour[4];
    };

    std::vector<Vertex> vertexBuffer;
    std::vector<unsigned int> indexBuffer;

    GLuint vbo; // Vertex buffer object.
    GLuint ibo; // Index buffer object.

    juce::String vertexShader;
    juce::String fragmentShader;

    std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLComponent)
};