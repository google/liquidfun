/*
* Copyright (c) 2013 Google, Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SANDBOX_H
#define SANDBOX_H

// Emits particles along a line, as if it were some kind of faucet.
class FaucetEmitter {
public:

	// World is a pointer to the world.
	// Origin is center of faucet
	// startingVelocity indicates starting velocity for particles
	// emitRate is particles/second
	// color is the particle color you want
	FaucetEmitter(b2ParticleSystem *particleSystem, const b2Vec2 &origin,
				  b2Vec2 &startingVelocity, float32 size, float32 emitRate,
				  b2ParticleColor color) :
		m_startingVelocity(startingVelocity), m_origin(origin), m_size(size),
		m_emitRate(emitRate), m_particleSystem(particleSystem), m_color(color)
	{
		m_counter = 0;
	}

	// dt is seconds that have passed, flags are the particle flags that you
	// want set on each particle.
	void Step(float32 dt, uint32 flags)
	{
		// How many (fractional) particles should we have emitted this frame?
		m_counter += m_emitRate * dt;

		b2ParticleDef pd;
		pd.color = m_color;

		// Keep emitting particles on this frame until we only have a
		// fractional particle left.
		while(m_counter > 1) {
			m_counter -= 1;

			// Randomly pick a position along the line that is the faucet.
			pd.position.Set(m_origin.x + (rand() % 100)/100.0f *
			                m_size - m_size/2, m_origin.y);
			// Send it flying
			pd.velocity = m_startingVelocity;
			pd.flags = flags;

			m_particleSystem->CreateParticle(pd);
		}
	}

 private:
	// Launch direction.
	b2Vec2 m_startingVelocity;
	// Center of particle emitter
	b2Vec2 m_origin;
	// Total width of particle emitter
	float32 m_size;
	// Particles per second
	float32 m_emitRate;

	// Pointer to global world
	b2ParticleSystem *m_particleSystem;
	// Color you'd like the faucet spray
	b2ParticleColor m_color;

	// Number particles I need to emit on the next frame
	float32 m_counter;
};

// The following parameters are not static const members of the Sandbox class
// with values assigned inline as it can result in link errors when using gcc.
namespace SandboxParams {

// Total possible pump squares
static const int MAX_PUMPS = 5;
// Total possible emitters
static const int MAX_EMITTERS = 5;
// Number of seconds to push one direction or the other on the pumps
static const float32 FLIP_TIME = 6;
// Radius of a tile
static const float32 TILE_RADIUS = 2;
// Diameter of a tile
static const float32 TILE_DIAMETER = 4;
// Pump radius; slightly smaller than a tile
static const float32 PUMP_RADIUS = 2.0f - 0.05f;

static const float32 PLAYFIELD_LEFT_EDGE = -20;
static const float32 PLAYFIELD_RIGHT_EDGE = 20;
static const float32 PLAYFIELD_BOTTOM_EDGE = 40;

// The world size in the TILE
static const int TILE_WIDTH = 10;
static const int TILE_HEIGHT = 11;

// Particles/second
static const float32 DEFAULT_EMITTER_RATE = 30;
// Fit cleanly inside one block
static const float32 DEFAULT_EMITTER_SIZE = 3;
// How fast particles coming out of the particles should drop
static const float32 PARTICLE_EXIT_Y_SPEED = -9.8f;
// How hard the pumps can push
static const float32 PUMP_FORCE = 600;

}  // namespace SandboxParams


// Sandbox test creates a maze of faucets, pumps, ramps, circles, and blocks
// based on a string constant.  Please modify and play with this string to make
// new mazes, and also add new maze elements!
class Sandbox : public Test
{
public:

	Sandbox()
	{
		using namespace SandboxParams;

		// We need some ground for the pumps to slide against
		b2BodyDef bd;
		b2Body* ground = m_world->CreateBody(&bd);

		// Reset our pointers
		for (int i = 0; i < MAX_EMITTERS; i++) {
			m_emitters[i] = NULL;
		}

		for (int i = 0; i < MAX_PUMPS; i++) {
			m_pumps[i] = NULL;
		}

		m_world->SetGravity(b2Vec2(0.0, -20));

		// Create physical box, no top
		{
			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-40, -10),
					b2Vec2(40, -10),
					b2Vec2(40, 0),
					b2Vec2(-40, 0)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(PLAYFIELD_LEFT_EDGE - 20, -1),
					b2Vec2(PLAYFIELD_LEFT_EDGE, -1),
					b2Vec2(PLAYFIELD_LEFT_EDGE, 50),
					b2Vec2(PLAYFIELD_LEFT_EDGE - 20, 50)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(PLAYFIELD_RIGHT_EDGE, -1),
					b2Vec2(PLAYFIELD_RIGHT_EDGE + 20, -1),
					b2Vec2(PLAYFIELD_RIGHT_EDGE + 20, 50),
					b2Vec2(PLAYFIELD_RIGHT_EDGE, 50)};
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}
		}

		m_particleSystem->SetParticleRadius(0.25f);

		m_pumpTimer = 0;

		SetupMaze();

		// Create killfield shape and transform
		m_killfieldShape = b2PolygonShape();
		m_killfieldShape.SetAsBox(PLAYFIELD_RIGHT_EDGE -
								  PLAYFIELD_LEFT_EDGE,1);

		// Put this at the bottom of the world
		m_killfieldTransform = b2Transform();
		b2Vec2 loc = b2Vec2(-20, 1);
		m_killfieldTransform.Set(loc, 0);

		// Setup particle parameters.
		TestMain::SetParticleParameters(k_paramDef, k_paramDefCount);
		m_particleFlags = TestMain::GetParticleParameterValue();
		TestMain::SetRestartOnParticleParameterChange(false);
	}

	~Sandbox()
	{
		// deallocate our emitters
		for (int i = 0; i < m_faucetEmitterIndex; i++) {
			delete m_emitters[i];
		}
	}


	// Create a maze of blocks, ramps, pumps, and faucets.
	// The maze is defined in a string; feel free to modify it.
	// Items in the maze include:
	//   # = a block
	//   / = a right-to-left ramp triangle
	//   A = a left-to-right ramp triangle (can't be \ or string formatting
	//       would be weird)
	//   r, g, b = colored faucets pointing down
	//   p = a pump block that rocks back and forth.  You can drag them
	//       yourself with your finger.
	//   C = a loose circle
	//   K = an ignored placeholder for a killfield to remove particles;
	//       entire bottom row is a killfield.
	void SetupMaze() {
		using namespace SandboxParams;

		const char *maze =
			"# r#g #r##"
			"  /#  #  #"
			" ###     p"
			"A  #  /###"
			"## # /#  C"
			"  /# #   #"
			" ### # / #"
			" ## p /#  "
			" #  ####  "
			"A        /"
			"#####KK###";

		b2Assert(strlen(maze) == TILE_WIDTH * TILE_HEIGHT);

		m_faucetEmitterIndex = 0;
		m_pumpIndex = 0;

		// Set up some standard shapes/vertices we'll use later.
		b2PolygonShape boxShape;
		boxShape.SetAsBox(TILE_RADIUS, TILE_RADIUS);

		b2Vec2 triangle[3];
		triangle[0].Set(-TILE_RADIUS, -TILE_RADIUS);
		triangle[1].Set(TILE_RADIUS, TILE_RADIUS);
		triangle[2].Set(TILE_RADIUS, -TILE_RADIUS);
		b2PolygonShape rightTriangleShape;
		rightTriangleShape.Set(triangle, 3);

		triangle[1].Set(-TILE_RADIUS, TILE_RADIUS);
		b2PolygonShape leftTriangleShape;
		leftTriangleShape.Set(triangle, 3);

		// Make these just a touch smaller than a tile
		b2CircleShape circleShape = b2CircleShape();
		circleShape.m_radius = TILE_RADIUS * 0.7f;

		b2ParticleColor red = b2ParticleColor(255, 128, 128, 255);
		b2ParticleColor green = b2ParticleColor(128, 255, 128, 255);
		b2ParticleColor blue = b2ParticleColor(128, 128, 255, 255);

		m_pumpForce = b2Vec2(PUMP_FORCE,0);

		for (int i = 0; i < TILE_WIDTH; i++) {
			for (int j = 0; j < TILE_HEIGHT; j++) {
				char item = maze[j * TILE_WIDTH + i];

				// Calculate center of this square
				b2Vec2 center = b2Vec2(
					PLAYFIELD_LEFT_EDGE + TILE_RADIUS * 2 * i + TILE_RADIUS,
					PLAYFIELD_BOTTOM_EDGE - TILE_RADIUS * 2 * j + TILE_RADIUS);

				// Let's add some items
				switch (item) {
				case '#':
					// Block
					CreateBody(center, boxShape, b2_staticBody);
					break;
				case 'A':
					// Left-to-right ramp
					CreateBody(center, leftTriangleShape, b2_staticBody);
					break;
				case '/':
					// Right-to-left ramp
					CreateBody(center, rightTriangleShape, b2_staticBody);
					break;
				case 'C':
					// A circle to play with
					CreateBody(center, circleShape, b2_dynamicBody);
					break;
				case 'p':
					AddPump(center);
					break;
				case 'b':
					// Blue emitter
					AddFaucetEmitter(center, blue);
					break;
				case 'r':
					// Red emitter
					AddFaucetEmitter(center, red);
					break;
				case 'g':
					// Green emitter
					AddFaucetEmitter(center, green);
					break;
				default:
					// add nothing
					break;
				}
			}
		}
	}

	void CreateBody(b2Vec2 &center, b2Shape &shape, b2BodyType type)
	{
		b2BodyDef def = b2BodyDef();
		def.position = center;
		def.type = type;
		b2Body *body = m_world->CreateBody(&def);
		body->CreateFixture(&shape, 10.0);
	}

	// Inititalizes a pump and its prismatic joint, and adds it to the world
	void AddPump(const b2Vec2 &center)
	{
		using namespace SandboxParams;

		// Don't make too many pumps
		b2Assert(m_pumpIndex < MAX_PUMPS);

		b2PolygonShape shape = b2PolygonShape();
		shape.SetAsBox(PUMP_RADIUS, PUMP_RADIUS);

		b2BodyDef def = b2BodyDef();
		def.position = center;
		def.type = b2_dynamicBody;
		def.angle = 0;
		b2Body *body = m_world->CreateBody(&def);
		body->CreateFixture(&shape, 5.0);

		// Create a prismatic joint and connect to the ground, and have it
		// slide along the x axis.
		b2PrismaticJointDef prismaticJointDef;
		prismaticJointDef.bodyA = m_groundBody;
		prismaticJointDef.bodyB = body;
		prismaticJointDef.collideConnected = false;
		prismaticJointDef.localAxisA.Set(1,0);
		prismaticJointDef.localAnchorA = center;

		m_world->CreateJoint(&prismaticJointDef);

		m_pumps[m_pumpIndex] = body;
		m_pumpIndex++;
	}

	// Initializes and adds a faucet emitter
	void AddFaucetEmitter(const b2Vec2 &center, b2ParticleColor &color)
	{
		using namespace SandboxParams;

		// Don't make too many emitters
		b2Assert(m_faucetEmitterIndex < SandboxParams::MAX_PUMPS);

		b2Vec2 startingVelocity = b2Vec2(0, PARTICLE_EXIT_Y_SPEED);

		m_emitters[m_faucetEmitterIndex] =
			new FaucetEmitter(m_particleSystem, center, startingVelocity,
							  DEFAULT_EMITTER_SIZE, DEFAULT_EMITTER_RATE,
							  color);

		m_faucetEmitterIndex++;
	}

	// Per-frame step updater overridden from Test
	virtual void Step(Settings *settings)
	{
		using namespace SandboxParams;

		Test::Step(settings);

		m_particleFlags = TestMain::GetParticleParameterValue();

		float32 dt = 1.0f/settings->hz;

		// Step all the emitters
		for (int i = 0; i < m_faucetEmitterIndex; i++)
		{
			FaucetEmitter *emitter = m_emitters[i];
			emitter->Step(dt, m_particleFlags);
		}

		// Do killfield work--kill every particle near the bottom of the screen
		m_particleSystem->DestroyParticlesInShape(m_killfieldShape,
												  m_killfieldTransform);

		// Move the pumps
		for (int i = 0; i < m_pumpIndex; i++)
		{
			b2Body* pump = m_pumps[i];

			// Pumps can and will clog up if the pile of particles they're
			// trying to push is too heavy. Increase PUMP_FORCE to make
			// stronger pumps.
			pump->ApplyForceToCenter(m_pumpForce, true);

			m_pumpTimer+=dt;

			// Reset pump to go back right again
			if (m_pumpTimer > FLIP_TIME) {
				m_pumpTimer -= FLIP_TIME;
				m_pumpForce.x *= -1;
			}
		}

		m_debugDraw.DrawString(
			5, m_textLine, "Keys: (a) zero out (water), (q) powder");
		m_textLine += DRAW_STRING_NEW_LINE;
		m_debugDraw.DrawString(
			5, m_textLine, "      (t) tensile, (v) viscous");
		m_textLine += DRAW_STRING_NEW_LINE;
	}

	// Allows you to set particle flags on devices with keyboards
	void Keyboard(unsigned char key)
	{
		uint32 toggle = 0;
		switch (key)
		{
			case 'a':
				m_particleFlags = 0;
				break;
			case 'q':
				toggle = b2_powderParticle;
				break;
			case 't':
				toggle = b2_tensileParticle;
				break;
			case 'v':
				toggle = b2_viscousParticle;
				break;
			case 'w':
				toggle = b2_wallParticle;
				break;
		}
		if (toggle) {
			if (m_particleFlags & toggle) {
				m_particleFlags = m_particleFlags & ~toggle;
			} else {
				m_particleFlags = m_particleFlags | toggle;
			}
		}
		TestMain::SetParticleParameterValue(m_particleFlags);
	}

	static Test* Create()
	{
		return new Sandbox;
	}

private:
	// Count of faucets in the world
	int m_faucetEmitterIndex;
	// Count of pumps in the world
	int m_pumpIndex;

	// How long have we been pushing the pumps?
	float32 m_pumpTimer;
	// Particle creation flags
	uint32 m_particleFlags;

	// Pump force
	b2Vec2 m_pumpForce;

	// The shape we will use for the killfield
	b2PolygonShape m_killfieldShape;
	// Transform for the killfield shape
	b2Transform m_killfieldTransform;

	// Pumps and emitters
	b2Body* m_pumps[SandboxParams::MAX_PUMPS];
	FaucetEmitter *m_emitters[SandboxParams::MAX_EMITTERS];

	static const ParticleParameter::Value k_paramValues[];
	static const ParticleParameter::Definition k_paramDef[];
	static const uint32 k_paramDefCount;
};

const ParticleParameter::Value Sandbox::k_paramValues[] =
{
	{b2_waterParticle, ParticleParameter::k_DefaultOptions, "water"},
	{b2_waterParticle, ParticleParameter::k_DefaultOptions |
				ParticleParameter::OptionStrictContacts, "water (strict)" },
	{b2_powderParticle, ParticleParameter::k_DefaultOptions, "powder"},
	{b2_tensileParticle, ParticleParameter::k_DefaultOptions, "tensile"},
	{b2_viscousParticle, ParticleParameter::k_DefaultOptions, "viscous"},
	{b2_tensileParticle | b2_powderParticle,
		ParticleParameter::k_DefaultOptions,
		"tensile powder"},
	{b2_viscousParticle | b2_powderParticle,
		ParticleParameter::k_DefaultOptions,
		"viscous powder"},
	{b2_viscousParticle | b2_tensileParticle | b2_powderParticle,
		ParticleParameter::k_DefaultOptions, "viscous tensile powder"},
	{b2_viscousParticle | b2_tensileParticle,
		ParticleParameter::k_DefaultOptions,
		"tensile viscous water"}
};

const ParticleParameter::Definition Sandbox::k_paramDef[] =
{
	{ Sandbox::k_paramValues, B2_ARRAY_SIZE(Sandbox::k_paramValues) },
};
const uint32 Sandbox::k_paramDefCount =
	B2_ARRAY_SIZE(Sandbox::k_paramDef);


#endif
