#include "NeuralWarfareEngine.h"
#include <iostream>
#include "angleTools.h"

std::vector<NeuralWarfareEngine::Agent*> getNearestNeighbors(NeuralWarfareEngine* eng, Vec2 query, size_t count, KDTree<NeuralWarfareEngine::Agent>::Condition condition = [](const NeuralWarfareEngine::Agent* a) { return true; })
{
    KDTree<NeuralWarfareEngine::Agent>::MaxHeap maxHeap;
    for (NeuralWarfareEngine::Agent& agent : eng->agents)
    {
        float distance = (query - agent.pos).Length();
        if (maxHeap.size() < count)
        {
            maxHeap.emplace(distance, &agent);
        }
        else if (distance < maxHeap.top().first && condition(&agent))
        {
            maxHeap.pop();
            maxHeap.emplace(distance, &agent);
        }
    }
    std::vector<NeuralWarfareEngine::Agent*> neighbors;
    while (!maxHeap.empty()) {
        neighbors.push_back(maxHeap.top().second);
        maxHeap.pop();
    }
    return neighbors;
}

std::vector<NeuralWarfareEngine::Agent*> getInRange(NeuralWarfareEngine* eng, Vec2 query, float range, KDTree<NeuralWarfareEngine::Agent>::Condition condition = [](const NeuralWarfareEngine::Agent* a) { return true; })
{
    std::vector<NeuralWarfareEngine::Agent*> agents;
    for (NeuralWarfareEngine::Agent& agent : eng->agents)
    {
        if ((agent.pos - query).Length() < range && condition(&agent))
        {
            agents.push_back(&agent);
        }
    }
    return agents;
}

float agentSize = 4;

NeuralWarfareEngine::Agent::Agent(size_t teamId, Vec2 pos, float health, double dir) :
    teamId(teamId),
    spawnPos(pos),
    baseHealth(health),
    dir(dir)
{
    Reset();
}

NeuralWarfareEngine::Agent::~Agent()
{
}

void NeuralWarfareEngine::Agent::TakeAction(size_t action)
{
    switch (action)
    {
    case 1:
        dir += 0.1;
        break;
    case 2:
        dir += -0.1;
    default:
        break;
    }
}

void NeuralWarfareEngine::Agent::UpdatePos(float delta)
{
    dir = normalizeAngle(dir);
    pos.x += cos(dir) * delta;
    pos.y += sin(dir) * delta;
}

void NeuralWarfareEngine::Agent::Reset()
{
    pos = spawnPos;
    health = baseHealth;
}

void NeuralWarfareEngine::doCollision(Agent* agentA, Agent* agentB)
{
    Vec2 colVec = agentA->pos - agentB->pos;
    double colAngle = colVec.Direction();

    // Calculate the difference in direction angles, normalized to [-pi, pi]
    double diffA = normalizeAngle(colAngle - agentA->dir);
    double diffB = normalizeAngle(colAngle + std::numbers::pi - agentB->dir);

    if (diffA > diffB)
    {
        agentA->health = -720;
        std::vector<Agent*> nearestAllys = kdTree->FindNearestNeighbors(agentA->pos, 1,
            [agentA](const NeuralWarfareEngine::Agent* a)
            {
                return a->teamId == agentA->teamId && a != agentA && a->health > 0;
            });
        if (!nearestAllys.empty())
        {
            agentA->pos = nearestAllys.front()->pos;
        }
    }
    if (diffA < diffB)
    {
         agentB->health = -720;
         std::vector<Agent*> nearestAllys = kdTree->FindNearestNeighbors(agentB->pos, 1,
             [agentB](const NeuralWarfareEngine::Agent* a)
             {
                 return a->teamId == agentB->teamId && a != agentB && a->health > 0;
             });
         if (!nearestAllys.empty())
         {
             agentB->pos = nearestAllys.front()->pos;
         }
    }
}

NeuralWarfareEngine::NeuralWarfareEngine(std::mt19937& gen, Vec2 simSize) : gen(gen), simSize(simSize)
{
    UpdateKDTree();
}

NeuralWarfareEngine::~NeuralWarfareEngine()
{
    delete kdTree;
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
            agent.pos = -(agent.pos - agent.pos.Normalize() * (delta + 1));
        }

        if (agent.pos.x < -simSize.x) {
            agent.pos.x = -simSize.x;
            agent.dir = std::numbers::pi - agent.dir;
        }
        else if (agent.pos.x > simSize.x) {
            agent.pos.x = simSize.x;
            agent.dir = std::numbers::pi - agent.dir; 
        }

        if (agent.pos.y < -simSize.y) {
            agent.pos.y = -simSize.y;
            agent.dir = -agent.dir;
        }
        else if (agent.pos.y > simSize.y) {
            agent.pos.y = simSize.y;
            agent.dir = -agent.dir;
        }

        if (agent.health <= 0)
        {
            agent.health++;
            continue;
        }
        if (agent.health > 1)
        {
            agent.health--;
        }
        agent.UpdatePos(delta);
	}

    UpdateKDTree();

    std::uniform_int_distribution<size_t> dis(0, 100);
    DoCollisions(kdTree->root);

}

void NeuralWarfareEngine::Reset()
{
    for (Agent& agent : agents)
    {
        agent.Reset();
    }
}

void NeuralWarfareEngine::UpdateKDTree()
{
    if (kdTree != nullptr)
    {
        delete kdTree;
    }
    std::vector<NeuralWarfareEngine::Agent*> agentVector;
    for (NeuralWarfareEngine::Agent& agent : agents)
    {
        if (agent.health > 0)
        {
            agentVector.push_back(&agent);
        }
    }
    kdTree = new KDTree<Agent>(agentVector);

}

void NeuralWarfareEngine::DoCollisions(KDTree<Agent>::KDNode* node)
{
    if (!node) return;
    std::vector<NeuralWarfareEngine::Agent*> others;
    if (node->point->health > 0)
    {
        kdTree->FindRange(node, node->point->pos, agentSize * 2, 0, others,
            [node](const NeuralWarfareEngine::Agent* a)
            {
                return a->teamId != node->point->teamId && a->health > 0;
            });
    }
    for (Agent* agent : others)
    {
        doCollision(node->point, agent);
    }

    DoCollisions(node->left);
    DoCollisions(node->right);
}

size_t NeuralWarfareEngine::AddTeam(size_t numAgents, float health, Vec2 pos)
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
    std::uniform_real_distribution<float> radDis(0, std::numbers::pi * 2);
    for (size_t i = 0; i < numAgents; i++)
    {
        agents.push_back(Agent(teamid, pos, health, radDis(gen)));
    }
	return teamid;
}

void NeuralWarfareEngine::RemoveTeam(size_t teamID)
{
    std::list<Agent>::iterator iter = agents.begin();
    while (iter != agents.end())
    {
        if ((*iter).teamId = teamID)
        {
            iter = agents.erase(iter);
        }
        else
        {
            iter++;
        }
    }
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

/// <summary>
/// Function to generate a color based on the team ID
/// </summary>
Color generateTeamColor(size_t teamID) {
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
        DrawEllipse(drawCenter.x + agent.pos.x * drawScale.x, drawCenter.y + agent.pos.y * drawScale.y, agentSize * drawScale.x, agentSize * drawScale.y, teamColor);
	}
}
