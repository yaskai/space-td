#include <stddef.h>
#include <stdint.h>
#include "raylib.h"

#ifndef HANDLER_H_
#define HANDLER_H_

// Type used for indexing/count of entities and components, easy to change if needed
#define INT_N	int16_t

// How many component types there are
#define COMP_TYPE_COUNT 	31

// No component
#define COMP_NULL			-1

// Component mapping struct
// Has array containining indices of components. 
typedef struct {
	// Component types can be found with a 1 shifting by index 
	// Useful for referring to specific component types
	//
	// eg.   
	// component_id[0] = 99  
	// translates to:
	// component of type 1 << 0 has index 99 in it's pool  
	// so, 1 << 0 = 0x01 meaning:  
	// transform component for this mapping is located at transforms[99]
	INT_N component_id[COMP_TYPE_COUNT];

} ComponentMap;

// Base entity struct 
typedef struct {
	uint32_t components;
	INT_N id;

	uint8_t flags;

} Entity;

// Transform component 
#define COMP_TRANSFORM 0x01
typedef struct {
	Vector2 position;
	Vector2 velocity;
	Vector2 scale;

	float rotation;

} comp_Transform;

// Sprite component
#define COMP_SPRITE 0x02
typedef struct {
	uint16_t frame;
	uint16_t sprite_id; 

	float rotation;

	uint8_t flags;

} comp_Sprite;

// Handler struct 
// Stores all entity and component data
// Data is modified with 'ComponentUpdate()' functions
typedef struct {
	// Entity array
	Entity *entities;
	
	// Component mapping array	
	ComponentMap *comp_mappings;
	
	// Component arrays
	comp_Transform *comp_transforms;
	comp_Sprite *comp_sprites;

	// Count and capacity:
	// count for current number of 'thing'
	// capacity for maximum allowed memory allocated for array of 'thing'
	//
	// For entities:
	INT_N entity_count, entity_capacity;

	// For components:
	INT_N transform_count, transform_capacity;
	INT_N sprite_count, sprite_capacity;

} Handler;

// Initalize handler:
// Allocate memory for entity and component arrays,
// set pointers, defaults, etc.
void HandlerInit(Handler *handler, float dt);

// Update all systems
void HandlerUpdate(Handler *handler, float dt);

// Draw entities
// *NOTE:
// will be moved later to 'render.h',
// sprite draw requests will be sent to renderer.
// Renderer will process requests then draw them to buffer
void HandlerDraw(Handler *handler);

// Create a new entity,
// insert entity and it's components to respective arrays
void AddEntity(Handler *handler, uint32_t components);

// Compenent/systems update functions
void TransformsUpdate(Handler *handler, float dt);
void SpritesUpdate(Handler *handler, float dt);

#endif
