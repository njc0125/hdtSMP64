#pragma once

#include "../hdtSSEUtils/NetImmerseUtils.h"
#include "../hdtSSEUtils/FrameworkUtils.h"

#include "hdtSkyrimSystem.h"

#include "IEventListener.h"
#include "HookEvents.h"

#include <mutex>

namespace hdt
{
	class ActorManager
		: public IEventListener<ArmorAttachEvent>
		, public IEventListener<SkinSingleHeadGeometryEvent>
		, public IEventListener<SkinAllHeadGeometryEvent>
		, public IEventListener<FrameEvent>
		, public IEventListener<ShutdownEvent>
	{

	protected:
		struct Head
		{
			struct HeadPart
			{
				Ref<BSGeometry>                 headPart;
				Ref<NiNode>						baseNode;
				std::string						physicsFile;
				Ref<SkyrimSystem>				physics;
			};
			
			Ref<IString>						prefix;
			Ref<BSFaceGenNiNode>                headNode;
			std::vector<HeadPart>               headParts;
			std::unordered_map<IDStr, IDStr>	renameMap;
			std::unordered_map<IDStr, uint8_t>  nodeUseCount;
			bool                                isFullSkinning;
		};
		
		struct Armor
		{
			Ref<IString>						prefix;
			Ref<NiAVObject>						armorWorn;
			std::unordered_map<IDStr, IDStr>	renameMap;
			std::string							physicsFile;
			Ref<SkyrimSystem>						physics;
		};

		struct Skeleton
		{
			Ref<NiNode>			skeleton;
			Ref<NiNode>			npc;
			std::vector<Armor>	armors;
			Head head;

			void cleanArmor();
			void cleanHead();
			void clear();

			bool isActiveInScene() const;

			void scanHead();
			void updateHead(BSFaceGenNiNode* head, BSGeometry * geometry);

			static void doSkeletonMerge(NiNode* dst, NiNode* src, IString* prefix, std::unordered_map<IDStr, IDStr>& map);
			static void doSkeletonClean(NiNode* dst, IString* prefix);
			static NiNode* cloneNodeTree(NiNode* src, IString * prefix, std::unordered_map<IDStr, IDStr>& map);
			static void renameTree(NiNode* root, IString * prefix, std::unordered_map<IDStr, IDStr>& map);

			// TODO: refactor this is just to get it working for now
			static void doHeadSkeletonMerge(NiNode* dst, NiNode* src, IString* prefix, std::unordered_map<IDStr, IDStr>& map, std::unordered_map<IDStr, uint8_t>& useMap);
			static void renameHeadTree(NiNode* root, IString* prefix, std::unordered_map<IDStr, IDStr>& map, std::unordered_map<IDStr, uint8_t>& useMap);
			static NiNode* cloneHeadNodeTree(NiNode* src, IString* prefix, std::unordered_map<IDStr, IDStr>& map, std::unordered_map<IDStr, uint8_t>& useMap);
			
		};

		bool					m_shutdown = false;
		std::recursive_mutex	m_lock;
		std::vector<Skeleton>	m_skeletons;

		Skeleton& getSkeletonData(NiNode* skeleton);

	public:
		ActorManager();
		~ActorManager();

		static ActorManager* instance();
		static IDStr generatePrefix(NiAVObject* armor);

		virtual void onEvent(const ArmorAttachEvent& e) override;
		virtual void onEvent(const FrameEvent& e) override;
		virtual void onEvent(const ShutdownEvent&) override;
		void onEvent(const SkinSingleHeadGeometryEvent&) override;
		void onEvent(const SkinAllHeadGeometryEvent&) override;

		void reloadMeshes();

		std::vector<Skeleton> getSkeletons() const;


	};
}