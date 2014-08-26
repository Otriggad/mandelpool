/**
 * @file sfml.cpp
 * @author Christofer Lind, Sebastian Rautila, Adam Risberg, Andreas Widmark
 * @date 7/6 2014
 * @brief GUI for mandelbrot using the SFML library.
 */


#include <stdio.h>
#include <sstream>
#include <SFML/Graphics.hpp>
extern "C" {
#include "../include/mandelbrot.h"
}

using namespace sf;

int main()
{
  unsigned int width = 700, height = 700;
  int dimensions = width*height, i = 0, iterations = 1000;
  double x = 0, y = 0, zoom = 0.5; 
  float a = 550, b = 200;

  colorPalette * c = color_createPalette(7);
  color_setColor(c, 0, 0, 0, 0);
  color_setColor(c, 0, 15, 120, 1);
  color_setColor(c, 0, 125, 186, 2);
  color_setColor(c, 245, 234, 102, 3);
  color_setColor(c, 0, 125, 186, 4);
  color_setColor(c, 0, 15, 120, 5);
  color_setColor(c, 0, 0, 0, 6);

  //class RenderWindow with right list
  sf::RenderWindow window(sf::VideoMode(width,height), "RenderWindow window");
  
  //class Vertex 
  sf::Vertex vertex(sf::Vector2f(10, 50), sf::Color::Red, sf::Vector2f(100, 100)); 
  
  //mandeldata struct
  struct mandelData * d = mandel_createMandelData(iterations, x-1/zoom, y+1/zoom, x+1/zoom, y-1/zoom, width, height, c);

  // render first image
  renderThread * currentRender = mandel_renderUnfinished(d, 4, 2);
  unsigned int *pixels = currentRender->image;

  while (window.isOpen())
    {
      sf::Event event;
      while (window.pollEvent(event))
        {
	  // close window
	  if (event.type == sf::Event::Closed)
	    window.close();
	  if (event.type == sf::Event::MouseButtonPressed) {
	    if(event.mouseButton.button == sf::Mouse::Left) {
	      int mx = event.mouseButton.x;
	      int my = event.mouseButton.y;
	      
	      double dx = (double)mx / (double)width;
	      double dy = (double)my / (double)height;
	      
	      x = x - 1.0/zoom + dx * (2.0/zoom);
	      y = y + 1.0/zoom - dy * (2.0/zoom);
	      zoom *= 2.0;

	      // free resources form old image
	      pthread_join(currentRender->thread, NULL);
	      free(currentRender);
	      mandel_destroyMandelData(d);

	      // render new image
	      d = mandel_createMandelData(iterations, x-1/zoom, y+1/zoom, x+1/zoom, y-1/zoom, width, height, c);
	      currentRender = mandel_renderUnfinished(d, 4, 16);
	      pixels = currentRender->image;
	    }
	    if(event.mouseButton.button == sf::Mouse::Right) {
	      int mx = event.mouseButton.x;
	      int my = event.mouseButton.y;
	      
	      double dx = (double)mx / (double)width;
	      double dy = (double)my / (double)height;
	      
	      x = x - 1.0/zoom + dx * (2.0/zoom);
	      y = y + 1.0/zoom - dy * (2.0/zoom);
	      zoom *= 0.5;

	      // free resources form old image
	      pthread_join(currentRender->thread, NULL);
	      free(currentRender);
	      mandel_destroyMandelData(d);

	      // render new image
	      d = mandel_createMandelData(iterations, x-1/zoom, y+1/zoom, x+1/zoom, y-1/zoom, width, height, c);
	      currentRender = mandel_renderUnfinished(d, 4, 16);
	      pixels = currentRender->image;
	    }

	  }
        }

      // set black background color
      window.clear(sf::Color(80,80,80));
      sf::Image image;
      
      image.create(width, height, (const Uint8*)pixels);

      sf::Texture texture;
      sf::Rect<int> r(0, 0, width, height);
      texture.loadFromImage(image, r);
	
      sf::Sprite sprite;
      sprite.setTexture(texture);
      
      window.draw(sprite);
      window.display();

      sf::sleep(sf::milliseconds(100));
    }

  pthread_join(currentRender->thread, NULL);
  free(currentRender);
  color_destroyPalette(c);
  mandel_destroyMandelData(d);
  return 0;
}
