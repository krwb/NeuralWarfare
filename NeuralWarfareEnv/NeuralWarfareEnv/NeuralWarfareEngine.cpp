#include "NeuralWarfareEngine.h"
NeuralWarfareEngine::Agent::Agent(size_t teamId) : teamId(teamId)
{
}

NeuralWarfareEngine::Agent::~Agent()
{
}

void NeuralWarfareEngine::Agent::TakeAction(size_t action)
{
    dir = Vec2();
    switch (action)
    {
    case 1:
        dir.x = 1;
        break;
    case 2:
        dir.y = 1;
        break;
    case 3:
        dir.x = -1;
        break;
    case 4:
        dir.y = -1;
        break;
    default:
        break;
    }
}

void NeuralWarfareEngine::Agent::UpdatePos(float delta)
{
    pos += dir * delta;
}

NeuralWarfareEngine::NeuralWarfareEngine(Vec2 simSize) : simSize(simSize)
{
    
}

NeuralWarfareEngine::~NeuralWarfareEngine()
{
}

void NeuralWarfareEngine::Update(float delta)
{
	for (Agent& agent : agents)
	{
        if (agent.pos.x < -simSize.x ||
            agent.pos.y < -simSize.y ||
            agent.pos.x > simSize.x ||
            agent.pos.y > simSize.y)
        {
            agent.health = 0;
        }
        if (agent.health <= 0)
        {
            continue;
        }
        agent.UpdatePos(delta);

	}
}

size_t NeuralWarfareEngine::AddTeam(size_t numAgents)
{
	size_t teamid;
	if (agents.size() == 0)
	{
        teamid = 0;
	}
	else
	{
        teamid = agents.back().teamId + 1;
	}
    for (size_t i = 0; i < numAgents; i++)
    {
        agents.push_back(Agent(teamid));
    }
	return teamid;
}


/// <summary>
/// Helper function to convert HSL to RGB
/// </summary>
/// <param name="H">hue</param>
/// <param name="S">saturation</param>
/// <param name="L">lightness</param>
/// <returns></returns>
Color HSLToRGB(float H, float S, float L) {
    float C = (1.0f - std::abs(2.0f * L - 1.0f)) * S;
    float X = C * (1.0f - std::abs(fmod(H / 60.0f, 2) - 1.0f));
    float m = L - C / 2.0f;

    float r, g, b;

    if (H >= 0.0f && H < 60.0f) {
        r = C; g = X; b = 0;
    }
    else if (H >= 60.0f && H < 120.0f) {
        r = X; g = C; b = 0;
    }
    else if (H >= 120.0f && H < 180.0f) {
        r = 0; g = C; b = X;
    }
    else if (H >= 180.0f && H < 240.0f) {
        r = 0; g = X; b = C;
    }
    else if (H >= 240.0f && H < 300.0f) {
        r = X; g = 0; b = C;
    }
    else {
        r = C; g = 0; b = X;
    }

    Color color;
    color.r = static_cast<unsigned char>((r + m) * 255.0f);
    color.g = static_cast<unsigned char>((g + m) * 255.0f);
    color.b = static_cast<unsigned char>((b + m) * 255.0f);
    color.a = 255; // Full opacity

    return color;
}

// Function to generate a color based on the team ID
Color generateTeamColor(int teamID) {
    // Normalize the team ID to a hue value (0-360 degrees)
    const float goldenRatioConjugate = 1.61803398874989;
    float hue = fmod((teamID * goldenRatioConjugate * 360.0f), 360.0f);

    float saturation = 0.7f; // Keep saturation constant
    float lightness = 0.5f;  // Keep lightness constant

    return HSLToRGB(hue, saturation, lightness);
}

void NeuralWarfareEngine::Draw(Rectangle drawRec)
{
    size_t lastTeamId = 0;
    Color teamColor = generateTeamColor(lastTeamId);
    Vec2 drawScale = Vec2(drawRec.width, drawRec.height) / simSize / 2;
    Vec2 drawCenter(drawRec.x + drawRec.width / 2, drawRec.y + drawRec.height / 2);
	for (Agent& agent : agents)
	{
        if (agent.health <= 0)
        {
            continue;
        }
        if (agent.teamId != lastTeamId)
        {
            lastTeamId = agent.teamId;
            teamColor = generateTeamColor(lastTeamId);
        }
        DrawCircle(drawCenter.x + agent.pos.x * drawScale.x, drawCenter.y + agent.pos.y * drawScale.y, 2, teamColor);
	}
}

