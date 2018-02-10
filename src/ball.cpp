#include "ball.h"
#include "main.h"

Ball::Ball(float x, float y, float radius, int semi, color_t color) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    speed = 0.01;
    rad = radius;
    int i;
    int n = 360;
    int counter = 0;
    GLfloat vertex_buffer_data[n*9];    
    if(semi){    
        for(i=0,counter=0;i<n;i++,counter+=9){
        vertex_buffer_data[counter]=(GLfloat)0;
        vertex_buffer_data[counter+1]=(GLfloat)0;
        vertex_buffer_data[counter+2]=(GLfloat)0;
        vertex_buffer_data[counter+3] = (GLfloat) radius*cos(i*2*3.1416/n);
        vertex_buffer_data[counter+4] = (GLfloat) radius*sin(i*2*3.1416/n);
        vertex_buffer_data[counter+5] = (GLfloat) 0;
        vertex_buffer_data[counter+6] = (GLfloat) radius*cos((i+1)*2*3.1416/n);
        vertex_buffer_data[counter+7] = (GLfloat) radius*sin((i+1)*2*3.1416/n);
        vertex_buffer_data[counter+8] = (GLfloat) 0;
        }

        for(i=0,counter=0;i<n/2;i++,counter+=9){
        vertex_buffer_data[counter]=(GLfloat)0;
        vertex_buffer_data[counter+1]=(GLfloat)0;
        vertex_buffer_data[counter+2]=(GLfloat)0;
        vertex_buffer_data[counter+3] = (GLfloat) 0;
        vertex_buffer_data[counter+4] = (GLfloat) 0;
        vertex_buffer_data[counter+5] = (GLfloat) 0;
        vertex_buffer_data[counter+6] = (GLfloat) 0;
        vertex_buffer_data[counter+7] = (GLfloat) 0;
        vertex_buffer_data[counter+8] = (GLfloat) 0;
        }
    }
    else{
        for(i=0,counter=0;i<n;i++,counter+=9){
        vertex_buffer_data[counter]=(GLfloat)0;
        vertex_buffer_data[counter+1]=(GLfloat)0;
        vertex_buffer_data[counter+2]=(GLfloat)0;
        vertex_buffer_data[counter+3] = (GLfloat) radius*cos(i*2*3.1416/n);
        vertex_buffer_data[counter+4] = (GLfloat) radius*sin(i*2*3.1416/n);
        vertex_buffer_data[counter+5] = (GLfloat) 0;
        vertex_buffer_data[counter+6] = (GLfloat) radius*cos((i+1)*2*3.1416/n);
        vertex_buffer_data[counter+7] = (GLfloat) radius*sin((i+1)*2*3.1416/n);
        vertex_buffer_data[counter+8] = (GLfloat) 0;
        }
    }
    

    this->object = create3DObject(GL_TRIANGLES, 3*n, vertex_buffer_data, color, GL_FILL);

}


void Ball::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Ball::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Ball::tick() {
    this->position.x -= speed;
    // this->position.y -= speed;
}

bounding_box_t Ball::bounding_box() {
    float x = this->position.x, y = this->position.y;
    bounding_box_t bbox = { x, y, 0.4, 0.4 };
    return bbox;
}
