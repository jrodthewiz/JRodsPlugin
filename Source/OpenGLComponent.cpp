/*
  ==============================================================================

    OpenGLComponent.cpp
    Created: 25 Jan 2023 11:54:17pm
    Author:  wolfk

  ==============================================================================
*/

#include <iostream>
#include "OpenGLComponent.h"

OpenGLComponent::~OpenGLComponent()
{
    // Tell the context to stop using this Component.
    openGLContext.detach();
}

OpenGLComponent::OpenGLComponent()
{
    // Indicates that no part of this Component is transparent.
    setOpaque(true);

    // Set this instance as the renderer for the context.
    openGLContext.setRenderer(this);

    // Tell the context to repaint on a loop.
    openGLContext.setContinuousRepainting(true);

    juce::OpenGLPixelFormat pixelFormat;
    pixelFormat.multisamplingLevel = 4; // Change this value to your needs.

    openGLContext.setPixelFormat(pixelFormat);

    // Finally - we attach the context to this Component.
    openGLContext.attachTo(*this);

    time = 0.0001;
}

void OpenGLComponent::newOpenGLContextCreated()
{

    // Generate 1 buffer, using our vbo variable to store its ID.
    openGLContext.extensions.glGenBuffers(1, &vbo);

    // Generate 1 more buffer, this time using our IBO variable.
    openGLContext.extensions.glGenBuffers(1, &ibo);

    vertexBuffer = {
        // Vertex 0
        {
            { -20.5f, 20.5f },        // (-0.5, 0.5)
            { 1.f, 0.f, 0.f, 1.f }  // Red
        },
        // Vertex 1
        {
            { 20.5f, 20.5f },         // (0.5, 0.5)
            { 1.f, 0.5f, 0.f, 1.f } // Orange
        },
        // Vertex 2
        {
            { 20.5f, -20.5f },        // (0.5, -0.5)
            { 1.f, 1.f, 0.f, 1.f }  // Yellow
        },
        // Vertex 3
        {
            { -20.5f, -20.5f },       // (-0.5, -0.5)
            { 1.f, 0.f, 1.f, 1.f }  // Purple
        }
    };

    // We need 6 indices, 1 for each corner of the two triangles.
    indexBuffer = {
        0, 1, 2,
        0, 2, 3
    };

    // Bind the VBO.
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Send the vertices data.
    openGLContext.extensions.glBufferData(
        GL_ARRAY_BUFFER,                        // The type of data we're sending.           
        sizeof(Vertex) * vertexBuffer.size(),   // The size (in bytes) of the data.
        vertexBuffer.data(),                    // A pointer to the actual data.
        GL_STATIC_DRAW                          // How we want the buffer to be drawn.
    );

    // Bind the IBO.
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // Send the indices data.
    openGLContext.extensions.glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * indexBuffer.size(),
        indexBuffer.data(),
        GL_STATIC_DRAW
    );

    vertexShader =
        R"(
            #version 330 core
            // Input attributes.
            in vec4 position;      
            in vec4 sourceColour;
           
            uniform float JRodModulator;
            
            // Output to fragment shader.
            out vec4 fragColour;
            out vec4 vertexPosition;
            
            void main()
            {
                // Set the position to the attribute we passed in.
                gl_Position = position;
                vertexPosition = position;
                
                // Set the frag colour to the attribute we passed in.
                // This value will be interpolated for us before the fragment
                // shader so the colours we defined ateach vertex will create a
                // gradient across the shape.
                //fragColour = sourceColour * vec4(JRodModulator, 0.5, 0.6, 0.5);
            }
        )";

   

    

    

    juce::String fractalShader =
        R"(
            #version 330 core
            
            // The value that was output by the vertex shader.
            // This MUST have the exact same name that we used in the vertex shader.
            uniform vec3 iResolution;
            uniform float iTime;      
            uniform float DialValue; //dial1    
            uniform float DialValue2;   //dial 2
            uniform float DialValue3;   //dial 3
            uniform float AudioData;  
            in vec4 vertexPosition;       

            float f(vec3 p) 
            {
	            float a, l, t;
    
                float factor = 1.0;
                float effect = -1.0 * 0.3;

	            for (int i = 0; i < 32; i++) 
                {
		            float m = dot(p, p);
		            p = abs(p) / m + 0.4 * vec2(cos(iTime/50.0), sin(iTime/50.0)).xxy - 0.5;
        
		            float w = exp(effect);
		            a += w * exp(-9.5 * pow(abs(m - l), 2.3));
		            t += w ;
                    //l = m;
                    factor = factor + 1.0;
	            }
    
	            return max(0.0, 5.0 * a / t - 0.7);
            }


            void main()
            {
                // Set the fragment's colour to the attribute passed in by the
                // vertex shader.
                vec3 p = (vec3(vertexPosition.xy, 0.0) - 0.5 * iResolution) / iResolution.y;
                gl_FragColor = pow(vec4(f(p)),vec4(1.0, 2.0, 3.0, 0.0)) * pow(vec4(f(p * 2.5)),vec4(3.0, 2.0, 1.0, 0.0)) * AudioData;// * DialValue;

                //vec3 R = iResolution, p = (vertexPosition.xyz - 0.5 * R) / R.y;
                //gl_FragColor = pow(vec4(f(p)),vec4(1.0, 2.0, 3.0, 0.0)) * pow(vec4(f(p * 2.5)),vec4(3.0, 2.0, 1.0, 0.0));
                
            }
        )";
    
    

    juce::String mandelBulb =
        R"(
          precision mediump float;
          
          

          const float radius = 0.01; //radius
      
          float mandelbulb(vec3 p) {
            float Bailout = 2.0;
            float Power;
            Power = 8.0 + (-1.0 * 6.0) * JRodModulator;
            //Power = 8.0;
            vec3 z = p;
            z = vec3(z.x, z.y, z.z);
            float dr = 1.0;
            float r = 0.0;
            for (int i = 0; i < 15; i++) {
              r = length(z);
              if (r>Bailout) break;

              // convert to polar coordinates
              //5, 3, 10, high mid low
              float test = float(i);
          
              //acos(z.z/r)
              float theta = (5.0 * JRodModulator) * acos(z.z/r);
              float phi = (3.0 * JRodModulator) * atan(z.y,z.x);
              dr = (JRodModulator * 10.0) * pow( r, Power-1.0)*Power*dr + 1.0;

              // scale and rotate the point
              float zr = pow( r,Power);
              theta = theta*Power;
              phi = phi*Power;

              // convert back to cartesian coordinates
              z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
              z+=p;
            }
            return 0.5*log(r)*r/dr;
        

          }

      

          float GetDist(vec3 p) {
            return mandelbulb(p);
          }

          void main() {


            vec3 curCameraRayUnit = normalize(vec3(randomInfluence+0.5, randomInfluence+0.5, randomInfluence+4.5) - (vec3(randomInfluence+3.5, randomInfluence+5.5, 0.0) - vec3(0.0, 0.0, 0.0)));


            // zero optimization done to step size, max iterations, etc..!
            const vec3 e=vec3(0.02,0,0);
            const float maxd=10.0; //Max depth
            vec3 p,N;

            //rotate the madelbulb
            vec3 model_position = vec3(randomInfluence, randomInfluence, randomInfluence);

            float sA,sP;

            // march to bg / ray marching loop
            vec3 color=vec3(1.0,1.0,1.0);
            float f=0.0; //starting value
            float d=0.001; //smallest distance before we break
            //100 vs 16
        
            for(int i=0;i<62;i++){
              if ((abs(d) < .001) || (f > maxd)) break;
              f+=d;
              p=model_position+curCameraRayUnit*f; //moving the ray forward
          
              //current distance of the ray, point has moved up, distance estimator applied

              d = GetDist(p); //9999
            }

            //specular parameters
            float specA=1.0, specP=10.0;

            if (f < maxd){

              //calculate the normal
              vec3 n = vec3(d-GetDist(p-e.xyy), //x is 0.2
                      d-GetDist(p-e.yxy),
                      d-GetDist(p-e.yyx));
              N = normalize(n);

              //calculate the light vector
              vec3 L = normalize(vec3(2.0, 4.0, 4.0)-p);

              //diffuse component, specular, and H?
              float nDotL=max(dot(N,L),0.0);

              vec3 H = normalize(L-curCameraRayUnit);

              float specular = max(dot(H,N),0.0);

              vec3 lightColor = vec3(1.0, 0.0, 0.0);

              //vec3 obj_color = vec3(0.9, 0.3, 0.3);

              vec3 obj_color = vec3(0.2, 0.3, 0.5);

              vec3 diffuse = lightColor * obj_color * nDotL;
              float specular_component = pow(specular,specP)*specA;
              //color = (nDotL*0.8+0.2)*vec3(0.0,1.0,0.0);

              vec3 ambient = obj_color;

              color = ambient + diffuse + specular_component;
            } else {
              //if ray too far then dont do the fragment
              discard;

            }

            //gl_FragColor = vec4(clamp(f_data, 0.0, 1.0), 0.0, 0.0, 1.0);
            gl_FragColor = vec4(color,1.0);

          }


        )";



    juce::String JuliaSet =
                R"(
        #version 330 core

        //https://www.shadertoy.com/view/Ndcczj            

        uniform vec3 iResolution;
        uniform float iTime;      
        uniform float DialValue; //dial1    
        uniform float DialValue2;   //dial 2
        uniform float DialValue3;   //dial 3
        uniform float AudioData;  
        in vec4 vertexPosition;


        void main()
        {
            vec2 z = ((2.0 * vertexPosition.xy + DialValue - iResolution.xy) / iResolution.y) * 10.5;
            float l = 0.0;
            for (l = 0.0; l < 100.0; l += 1.0) {
                z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + vec2(sin(iTime / 2.0), cos(iTime / 2.0));
                if(dot(z, z) > 65536.0) break;
            }
            gl_FragColor = vec4(vec3((l > 99.0) ? 0.0 : sin(l / 20.0)) * vec3(0.1, DialValue2, DialValue3), 1.0); // Samples - 1 check. Makes interior black

                
        }
    )";

    
    juce::String Mandelbrot =
                R"(
        #version 330 core
        #extension GL_ARB_gpu_shader_fp64 : enable
        //https://www.shadertoy.com/view/fscyzB
        //https://www.shadertoy.com/view/fdtcD2

        uniform vec3 iResolution;
        uniform float iTime;        
        uniform float DialValue; //dial1    
        uniform float DialValue2;   //dial 2
        uniform float DialValue3;   //dial 3
        uniform float AudioData;
        uniform float TimeStamp;
        uniform vec2 iMouse;
        in vec4 vertexPosition;

        vec3 colorAdder(float m) {
            return vec3(cos(m), cos(m), 1-cos(m));
        }

        vec3 hashRod(float m) {
            //m *= AudioData;
            float x = fract(sin(m) * 2625.246 * AudioData);
            float y = fract(cos(m + x) * 2216.486 * AudioData) + (AudioData / 100.0);
            float z = fract(sin(x + y * AudioData) * 8276.352) + (AudioData / 100.0) + DialValue2;
            return vec3(x, y, z);

        }

        vec3 hsv2rgb(vec3 c) {
            vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
            vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
            return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
        }

        vec3 colorGradientSimplex(float iterations, float maxiterations) {
            vec3 col;
            if (iterations < maxiterations / 4) {
                col = vec3(0.0, 0.0, 0.2);
            } else if (iterations < maxiterations / 3) {
                col = vec3(0.0, 0.5, 0.0);
            } else if (iterations < maxiterations / 2) {
                col = vec3(0.5, 0.0, 0.0);
            } else {
                col = vec3(0.9, 0.5, 0.1);
            }
            return col;
        }
        
        vec3 colorGradient(float iterations, float maxiterations) {
            return vec3(abs(cos(iterations / maxiterations * 3.1415)), abs(sin(iterations / maxiterations * 3.1415)), abs(cos(iterations / maxiterations * 3.1415)));
        }

        //DialValue1: 0.5;
        //DialValue2: -0.4;
        //DialValue3: -1.0;
        
        vec4 mandelbrot(vec2 uv){
            uv.x -= (0.5 - 1.0);
            uv.y -= (0.5 - 1.0);
            
            vec2 c = 4.0 * uv - vec2(-1.0, -0.4) / 100.0;
            
            //Center and zoom (-1 * 20 + 7, -1 * 20 + 25)
            float centerX = (-1 * 7.0 + 20.0) - DialValue;
            float centerY = -(-1 * 25.0 + 20.0);
            c = (c - vec2(centerX / iResolution.x, centerY / iResolution.y)) / pow(0.05, 2.0) + vec2(centerX / iResolution.x, centerY / iResolution.y);
            
            //c = c / pow(iTime, 1.0);
            
            //Mouse movement
            vec2 target = vec2(iMouse.x / iResolution.x, iMouse.y / iResolution.y);
            //vec2 target = vec2(0.0, 0.0);
            c = (c - target) / pow(iTime, 0.5) + target;            
            
            /*float zoomLevel = pow(iTime, 0.1);
            vec2 mousePos = vec2(iMouse.x / iResolution.x / zoomLevel, iMouse.y / iResolution.y / zoomLevel);

            c = (c - mousePos) / pow(iTime, 0.5) + mousePos;*/



            //c = (c - vec2(iMouse.x / iResolution.x, -iMouse.y / iResolution.y)) / pow(0.05, 2.0) + vec2(iMouse.x / iResolution.x, -iMouse.y / iResolution.y);
            
            //vec2 offset = vec2(0.65, 0.45);
            //c = (c - vec2(iMouse.x / iResolution.x, -iMouse.y / iResolution.y)) / pow(0.5, iTime) + vec2(iMouse.x / iResolution.x, -iMouse.y / iResolution.y);
            //c = c / pow(iTime, 1.0) - vec2(0.65, 0.45);
            //vec2 center = iResolution.xy / 2.0;
            //c = (c - (center / iResolution.xy)) / pow(0.5, 2.0) + (center / iResolution.xy);

            vec2 z = vec2(0.0);
            float l = 0.0;
            float iterations = 0.0 * AudioData;
            
            float maxiterations = 0.0;
            if (iTime > 50.0) { 
                maxiterations = 1000.0 * pow(1.1, 50.0);
            }
            else {
                maxiterations = 1000.0 * pow(1.1, iTime);
            }

            for (l = 0.0; l < 1000; l += 1.0) {

                //z = vec2(z.x * z.x - z.y * z.y,  (AudioData / 100.0) * 2.0 * z.x * z.y) + c; // z = z ^ 2 + c 
                //z = vec2(z.x * z.x * z.x - 3.0 * z.x * z.y * z.y, (AudioData / 100.0) * 3.0 * z.x * z.x * z.y - z.y * z.y * z.y) + c // z = z ^ 3 + c
                //z = vec2(z.x * z.x - z.y * z.y, (AudioData / 100.0) * -2.0 * z.x * z.y) + vec2(c.x, c.y * -1.0); // z = z ^ 2 + ci
                z = vec2(z.x * z.x - z.y * z.y, (AudioData / 100.0) * 2.0 * z.x * z.y) + c / (vec2(1.0, 1.0) + vec2(abs(z.x), abs(z.y)));

                if(dot(z, z) > 10000.0) return vec4(hashRod(iterations * AudioData), 1.0); //Colorful
                //if(dot(z, z) > 10000.0) return vec4(iterations * AudioData, iterations * AudioData, iterations * AudioData, 1.0);  //Black
                iterations++;
            } 

            return vec4(0.0, 0.0, 0.0, 0.0);
        }

        

        void main() {
         
            vec2 uv = (( vertexPosition.xy - 0.5 * iResolution.xy) / iResolution.y); 
            gl_FragColor = mandelbrot(uv);
            
        }
            


        //vec3 col = vec3(0.0, 0.0, 0.0);
        //col = vec3((l > 99.0) ? AudioData : sin(l / 20.0)) * vec3(low, mid + 0.9, high) * DialValue;
        //vec2 m = mandelbrot(uv);
        //col += hashRod(cos(m)); //
        //col += m; //
        //if (m == 0.0) {
        //    col = vec3(0.0, 0.0, 0.0);
        //} else {
        //    col = hsv2rgb(vec3(m, 1.0, 1.0));
        //}
        //gl_FragColor = vec4(colorGradient(m.x), 1.0);
        //gl_FragColor = vec4(col, 1.1); //DialValue


    )";

    // Create an instance of OpenGLShaderProgram
    shaderProgram.reset(new juce::OpenGLShaderProgram(openGLContext));

    // Compile and link the shader.
    // Each of these methods will return false if something goes wrong so we'll
    // wrap them in an if statement
    //&& shaderProgram->addFragmentShader(Mandelbrot)

    if (shaderProgram->addVertexShader(vertexShader) 
        && shaderProgram->addFragmentShader(Mandelbrot)
        && shaderProgram->link())
    {   //
        // No compilation errors - set the shader program to be active
        shaderProgram->use();
    }
    else
    {
        // Oops - something went wrong with our shaders!
        // Check the output window of your IDE to see what the error might be.
        jassertfalse;
    }



}



void OpenGLComponent::renderOpenGL()
{
    // Clear the screen by filling it with black.
    juce::OpenGLHelpers::clear(juce::Colours::black);
    shaderProgram->use();

    GLuint DialValueLocation = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "DialValue");
    GLuint DialValue2Location = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "DialValue2");
    GLuint DialValue3Location = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "DialValue3");
    GLuint AudioDataLocation = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "AudioData");
    GLuint TimeStampObj = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "TimeStamp");
    GLuint FFT_Mid_Location = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "mid");
    GLuint FFT_High_Location = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "high");
    GLint iResolution = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "iResolution");
    GLint iTime = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "iTime");
    GLint mousPosLocation = openGLContext.extensions.glGetUniformLocation(shaderProgram->getProgramID(), "iMouse");

    GLfloat randomInfluence = (float)rand() / RAND_MAX;


    GLfloat width = getWidth();
    GLfloat height = getHeight();
    GLfloat timeF = time;

    //DBG(time);
    if (time < 0.5) { time = 0.5; }
    //time = 0.8;

    

    if (maxIterations > 0) {
        //openai::start(); // Will use the api key provided by `OPENAI_API_KEY` environment variable
       /* openai::start("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", "optional_organization"); // Or you can handle it yourself

        auto completion = openai::completion().create(R"({
            "model": "text-davinci-003",
            "prompt": "Are you ready to compile my shader code?",
            "max_tokens": 7,
            "temperature": 0
        })"_json); // Using user-defined (raw) string literals
        std::cout << "Response is:\n" << completion.dump(2) << '\n';*/

    }

    maxIterations--;

    
    if (AudioData < 0) {
        AudioData = 100;
    }

    if (isMouseButtonDown()) {
    //    

        //mousePos = getMouseXYRelative();
        //time = time * (1.05);
    //    mousePos = juce::Point<int>(7, 25);

    //    std::string mousePosString = "(x, y): " + std::to_string(mousePos_.x) + " " + std::to_string(mousePos_.y);
    //    /*DBG(mousePosString);*/
    }
    
    openGLContext.extensions.glUniform3f(iResolution, width*2, height*2, 0);
    openGLContext.extensions.glUniform1f(iTime, time);
    openGLContext.extensions.glUniform1f(DialValue2Location, DialValue2);
    openGLContext.extensions.glUniform1f(DialValue3Location, DialValue3);
    openGLContext.extensions.glUniform1f(DialValueLocation, DialValue);
    openGLContext.extensions.glUniform2f(mousPosLocation, 1 * mousePos.x + 20.0, -1 * mousePos.y + 20.0);

    float audioDataScaled = AudioData * 100.0;
    //DBG(audioDataScaled);
    
    openGLContext.extensions.glUniform1f(TimeStampObj, TimeStamp);
    openGLContext.extensions.glUniform1f(FFT_Mid_Location, mid);
    openGLContext.extensions.glUniform1f(FFT_High_Location, high);
    
    std::string t = std::to_string(TimeStamp) + " " + std::to_string(mid) + " " + std::to_string(high);

    //DBG(t);
    openGLContext.extensions.glUniform1f(AudioDataLocation, audioDataScaled);
    //glUniform1f(myUniformLocation, 4.0);

    //time = time + 0.005;


    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // Enable the position attribute.
    openGLContext.extensions.glVertexAttribPointer(
        0,              // The attribute's index (AKA location).
        2,              // How many values this attribute contains.
        GL_FLOAT,       // The attribute's type (float).
        GL_FALSE,       // Tells OpenGL NOT to normalise the values.
        sizeof(Vertex), // How many bytes to move to find the attribute with
                        // the same index in the next vertex.
        nullptr         // How many bytes to move from the start of this vertex
                        // to find this attribute (the default is 0 so we just
                        // pass nullptr here).
    );
    openGLContext.extensions.glEnableVertexAttribArray(0);

    // Enable to colour attribute.
    openGLContext.extensions.glVertexAttribPointer(
        1,                              // This attribute has an index of 1
        4,                              // This time we have four values for the
                                        // attribute (r, g, b, a)
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (GLvoid*)(sizeof(float) * 2)    // This attribute comes after the
                                        // position attribute in the Vertex
                                        // struct, so we need to skip over the
                                        // size of the position array to find
                                        // the start of this attribute.
    );
    openGLContext.extensions.glEnableVertexAttribArray(1);

    glDrawElements(
        GL_TRIANGLES,       // Tell OpenGL to render triangles.
        indexBuffer.size(), // How many indices we have.
        GL_UNSIGNED_INT,    // What type our indices are.
        nullptr             // We already gave OpenGL our indices so we don't
                            // need to pass that again here, so pass nullptr.
    );

    openGLContext.extensions.glDisableVertexAttribArray(0);
    openGLContext.extensions.glDisableVertexAttribArray(1);

}

void OpenGLComponent::openGLContextClosing()
{
}

void OpenGLComponent::paint(juce::Graphics& g)
{
}

void OpenGLComponent::setMousePos(juce::Point<int> point) {
    mousePos = point;
}