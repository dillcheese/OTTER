#include "Bloom.h"

void Bloom::Init(unsigned width, unsigned height)
{
	int index = int(_buffers.size());
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->AddDepthTarget();
	_buffers[index]->Init(width, height);
	index++;
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->AddDepthTarget();
	_buffers[index]->Init(width, height);
	index++;
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->AddDepthTarget();
	_buffers[index]->Init(width, height);
	index++;
	
	//Loads the shaders
	int indexS = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[indexS]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[indexS]->LoadShaderPartFromFile("shaders/passthrough_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[indexS]->Link();
	indexS++;
	_shaders.push_back(Shader::Create());
	_shaders[indexS]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[indexS]->LoadShaderPartFromFile("shaders/Post/bloom_brightpass_frag.glsl", GL_FRAGMENT_SHADER); //gets bright colors
	_shaders[indexS]->Link();
	indexS++;
	_shaders.push_back(Shader::Create());
	_shaders[indexS]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[indexS]->LoadShaderPartFromFile("shaders/Post/blur_gaussian_frag.glsl", GL_FRAGMENT_SHADER); //horizontal and vertical gaussian blur
	_shaders[indexS]->Link();
	indexS++;
	_shaders.push_back(Shader::Create());
	_shaders[indexS]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[indexS]->LoadShaderPartFromFile("shaders/Post/bloom_combine_frag.glsl", GL_FRAGMENT_SHADER);//combines all effects
	_shaders[indexS]->Link();
	
	PostEffect::Init(width, height);
}

void Bloom::ApplyEffect(PostEffect* buffer)
{
	//Draws previous buffer to first render target
	BindShader(0);

	buffer->BindColorAsTexture(0, 0, 0);
	_buffers[0]->RenderToFSQ();
	buffer->UnbindTexture(0);

	UnbindShader();

	//Get birght colors on the first render target
	BindShader(1);
	_shaders[1]->SetUniform("u_Threshold", m_threshold);

	BindColorAsTexture(0, 0, 0);
	_buffers[1]->RenderToFSQ();
	UnbindTexture(0);

	UnbindShader();

	//blurs vertically and horizontally m_amount of times
	for (int i = 0; i < m_amount; i++)
	{
		//Horizontal pass
		BindShader(2);
		_shaders[2]->SetUniform("u_Horizontal", (int)true);
		//_shaders[2]->SetUniform("u_Blur", m_intensity);

		BindColorAsTexture(1, 0, 0);
		
		_buffers[2]->RenderToFSQ();

		UnbindTexture(0);
		
		//Vertical pass
		_shaders[2]->SetUniform("u_Horizontal", (int)false);//set to vertical

		BindColorAsTexture(2, 0, 0);

		_buffers[1]->RenderToFSQ();

		UnbindTexture(0);
		UnbindShader();
	}

	//combine the blur and scene to create bloom effect
	BindShader(3);
	
	_shaders[3]->SetUniform("u_Intensity", m_intensity);//exposure

	buffer->BindColorAsTexture(0, 0, 0);
	BindColorAsTexture(1, 0, 1);
	
	_buffers[0]->RenderToFSQ();

	UnbindTexture(1);
	UnbindTexture(0);
	UnbindShader();
}

float Bloom::GetIntensity() const
{
	return m_intensity;
}

float Bloom::GetThreshold() const
{
	return m_threshold;
}

int Bloom::GetAmount() const
{
	return m_amount;
}

void Bloom::SetIntensity(float intensity)
{
	m_intensity = intensity;
}

void Bloom::SetThreshold(float threshold)
{
	m_threshold = threshold;
}

void Bloom::SetAmount(unsigned amount)
{
	m_amount = amount;
}
