#pragma once

#include "Vector.h"
#include "VMatrix.h"
#include"ICVar.h"

typedef float Quaternion[4];
typedef float RadianEuler[3];

#define MAX_QPATH  260

#define BONE_CALCULATE_MASK             0x1F
#define BONE_PHYSICALLY_SIMULATED       0x01    // bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL         0x02    // procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL          0x04    // bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE        0x08    // bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER      0x10    // bone aligns to the screen, constrained by it's own axis.

#define BONE_USED_MASK                  0x0007FF00
#define BONE_USED_BY_ANYTHING           0x0007FF00
#define BONE_USED_BY_HITBOX             0x00000100    // bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT         0x00000200    // bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK        0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0        0x00000400    // bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1        0x00000800    
#define BONE_USED_BY_VERTEX_LOD2        0x00001000  
#define BONE_USED_BY_VERTEX_LOD3        0x00002000
#define BONE_USED_BY_VERTEX_LOD4        0x00004000
#define BONE_USED_BY_VERTEX_LOD5        0x00008000
#define BONE_USED_BY_VERTEX_LOD6        0x00010000
#define BONE_USED_BY_VERTEX_LOD7        0x00020000
#define BONE_USED_BY_BONE_MERGE         0x00040000    // bone is available for bone merge to occur against it

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8
#define MAXSTUDIOBONES		128		// total bones actually used

#define BONE_TYPE_MASK                  0x00F00000
#define BONE_FIXED_ALIGNMENT            0x00100000    // bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS          0x00200000    // Vector48
#define BONE_HAS_SAVEFRAME_ROT64        0x00400000    // Quaternion64
#define BONE_HAS_SAVEFRAME_ROT32        0x00800000    // Quaternion32

#define Assert( _exp ) ((void)0)

#define HITGROUP_GENERIC 0
#define HITGROUP_HEAD 1
#define HITGROUP_CHEST 2
#define HITGROUP_STOMACH 3
#define HITGROUP_LEFTARM 4    
#define HITGROUP_RIGHTARM 5
#define HITGROUP_LEFTLEG 6
#define HITGROUP_RIGHTLEG 7
#define HITGROUP_GEAR 10

enum modtype_t
{
	mod_bad = 0,
	mod_brush,
	mod_sprite,
	mod_studio
};

enum Hitboxes
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_STOMACH,
	HITBOX_LOWER_CHEST,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};

typedef unsigned short MDLHandle_t;

struct mstudiobone_t
{
	int                    sznameindex;
	inline char* const    pszName(void) const { return ((char*)this) + sznameindex; }
	int                    parent;
	int                    bonecontroller[6];    // bone controller index, -1 == none
	Vector                 pos;
	Quaternion             quat;
	RadianEuler            rot;
	// compression scale
	Vector                 posscale;
	Vector                 rotscale;

	matrix3x4_t            poseToBone;
	Quaternion             qAlignment;
	int                    flags;
	int                    proctype;
	int                    procindex;
	mutable int            physicsbone;
	inline void* pProcedure() const { if (procindex == 0) return NULL; else return  (void*)(((std::byte*)this) + procindex); };
	int                    surfacepropidx;
	inline char* const    pszSurfaceProp(void) const { return ((char*)this) + surfacepropidx; }
	inline int             GetSurfaceProp(void) const { return surfacepropLookup; }

	int                    contents;
	int                    surfacepropLookup;
	int                    m_iPad01[7];

	mstudiobone_t() {}
private:
	// No copy constructors allowed
	mstudiobone_t(const mstudiobone_t& vOther);
};


struct mstudiobbox_t
{
	int         bone;
	int         group;
	Vector      bbmin;
	Vector      bbmax;
	int         szhitboxnameindex;
	int32_t     m_iPad01[3];
	float       m_flRadius;
	int32_t     m_iPad02[4];

	char *getHitboxName()
	{
		if (szhitboxnameindex == 0)
			return "";

		return ((char*)this) + szhitboxnameindex;
	}
};

struct mstudiohitboxset_t
{
	int    sznameindex;
	int    numhitboxes;
	int    hitboxindex;

	inline char *const pszName( void ) const
	{
		return ((char*)this) + sznameindex;
	}

	inline mstudiobbox_t *pHitbox( int i ) const
	{
		if (i > numhitboxes) return nullptr;
		return (mstudiobbox_t*)((uint8_t*)this + hitboxindex) + i;
	}
};

struct model_t
{
	char name[255];
};

struct mstudioattachment_t
{
	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	unsigned int		flags;
	int					localbone;
	matrix3x4_t			local; // attachment point
	int					unused[8];
};

struct studiohdr_t {
	int id;
	int version;
	int checksum;        // this has to be the same in the phy and vtx files to load!
	char name[64];
	int length;

	Vector eyeposition;    // ideal eye position
	Vector illumposition;    // illumination center
	Vector hull_min;        // ideal movement hull size
	Vector hull_max;
	Vector view_bbmin;        // clipping bounding box
	Vector view_bbmax;

	int flags;
	int numbones;            // bones
	int boneindex;
	inline mstudiobone_t* pBone(int i) const
	{
		Assert(i >= 0 && i < numbones);
		return (mstudiobone_t*)(((unsigned char*)this) + boneindex) + i;
	};

	int RemapSeqBone(int iSequence, int iLocalBone) const;    // maps local sequence bone to global bone
	int RemapAnimBone(int iAnim, int iLocalBone) const;        // maps local animations bone to global bone
	int numbonecontrollers;        // bone controllers
	int bonecontrollerindex;
	int numhitboxsets;
	int hitboxsetindex;

	// Look up hitbox set by index
	mstudiohitboxset_t* pHitboxSet(int i) const
	{
		(i >= 0 && i < numhitboxsets);
		return (mstudiohitboxset_t*)(((unsigned char*)this) + hitboxsetindex) + i;
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t* pHitbox(int i, int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return NULL;

		return s->pHitbox(i);
	};

	// Calls through to set to get hitbox count for set
	inline int iHitboxCount(int set) const
	{
		mstudiohitboxset_t const* s = pHitboxSet(set);
		if (!s)
			return 0;

		return s->numhitboxes;
	};

	// file local animations? and sequences
	//private:
	int numlocalanim;            // animations/poses
	int localanimindex;        // animation descriptions
	int numlocalseq;                // sequences
	int localseqindex;

	//public:
	bool SequencesAvailable() const;

	int GetNumSeq() const;

	int iRelativeAnim(int baseseq, int relanim) const;    // maps seq local anim reference to global anim index
	int iRelativeSeq(int baseseq, int relseq) const;        // maps seq local seq reference to global seq index

	//private:
	mutable int activitylistversion;    // initialization flag - have the sequences been indexed?
	mutable int eventsindexed;

	//public:
	int GetSequenceActivity(int iSequence);

	void SetSequenceActivity(int iSequence, int iActivity);

	int GetActivityListVersion();

	void SetActivityListVersion(int version) const;

	int GetEventListVersion();

	void SetEventListVersion(int version);

	// raw textures
	int numtextures;
	int textureindex;

	// raw textures search paths
	int numcdtextures;
	int cdtextureindex;

	inline char* pCdtexture(int i) const
	{
		return (((char*)this) + *((int*)(((unsigned char*)this) + cdtextureindex) + i));
	};

	// replaceable textures tables
	int numskinref;
	int numskinfamilies;
	int skinindex;

	inline short* pSkinref(int i) const
	{
		return (short*)(((unsigned char*)this) + skinindex) + i;
	};
	int numbodyparts;
	int bodypartindex;

	// queryable attachable points
	//private:
	int numlocalattachments;
	int localattachmentindex;

	//public:
	int GetNumAttachments() const;

	int GetAttachmentBone(int i);

	// used on my tools in hlmv, not persistant
	void SetAttachmentBone(int iAttachment, int iBone);

	// animation node to animation node transition graph
	//private:
	int numlocalnodes;
	int localnodeindex;
	int localnodenameindex;

	inline char* pszLocalNodeName(int iNode) const
	{
		(iNode >= 0 && iNode < numlocalnodes);
		return (((char*)this) + *((int*)(((unsigned char*)this) + localnodenameindex) + iNode));
	}

	inline unsigned char* pLocalTransition(int i) const
	{
		(i >= 0 && i < (numlocalnodes * numlocalnodes));
		return (unsigned char*)(((unsigned char*)this) + localnodeindex) + i;
	};

	//public:
	int EntryNode(int iSequence);

	int ExitNode(int iSequence);

	char* pszNodeName(int iNode);

	int GetTransition(int iFrom, int iTo) const;

	int numflexdesc;
	int flexdescindex;
	int numflexcontrollers;
	int flexcontrollerindex;
	int numflexrules;
	int flexruleindex;
	int numikchains;
	int ikchainindex;
	int nummouths;
	int mouthindex;

	//private:
	int numlocalposeparameters;
	int localposeparamindex;

	//public:
	int GetNumPoseParameters() const;

	int GetSharedPoseParameter(int iSequence, int iLocalPose) const;

	int surfacepropindex;

	inline char* const pszSurfaceProp() const
	{
		return ((char*)this) + surfacepropindex;
	}

	// Key values
	int keyvalueindex;
	int keyvaluesize;

	inline const char* KeyValueText() const
	{
		return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : NULL;
	}

	int numlocalikautoplaylocks;
	int localikautoplaylockindex;

	int GetNumIKAutoplayLocks() const;

	int CountAutoplaySequences() const;

	int CopyAutoplaySequences(unsigned short* pOut, int outCount) const;

	int GetAutoplayList(unsigned short** pOut) const;

	// The collision model mass that jay wanted
	float mass;
	int contents;

	// external animations, models, etc.
	int numincludemodels;
	int includemodelindex;

	// implementation specific call to get a named model
	const studiohdr_t* FindModel(void** cache, char const* modelname) const;

	// implementation specific back pointer to virtual data
	mutable void* virtualModel;
	//virtualmodel_t		GetVirtualModel() const;

	// for demand loaded animation blocks
	int szanimblocknameindex;

	inline char* const pszAnimBlockName() const
	{
		return ((char*)this) + szanimblocknameindex;
	}

	int numanimblocks;
	int animblockindex;
	mutable void* animblockModel;

	unsigned char* GetAnimBlock(int i) const;

	int bonetablebynameindex;

	inline const unsigned char* GetBoneTableSortedByName() const
	{
		return (unsigned char*)this + bonetablebynameindex;
	}

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	void* pVertexBase;
	void* pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting
	// on static props
	unsigned char constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	unsigned char rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	unsigned char numAllowedRootLODs;
	unsigned char unused[1];
	int unused4; // zero out if version < 47
	int numflexcontrollerui;
	int flexcontrolleruiindex;
	int unused3[2];

	// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
	int studiohdr2index;

	// NOTE: No room to add stuff? Up the .mdl file format version
	// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
	// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
	int unused2[1];

	inline mstudioattachment_t* pLocalAttachment(int i) const { Assert(i >= 0 && i < numlocalattachments); return (mstudioattachment_t*)(((BYTE*)this) + localattachmentindex) + i; };

	studiohdr_t() { }
private:
	// No copy constructors allowed
	studiohdr_t(const studiohdr_t& vOther);

	friend struct virtualmodel_t;
};//Size=0x00D4

class virtualgroup_t
{
public:
	void* cache;
	const studiohdr_t* GetStudioHdr(void) const
	{
		return (studiohdr_t*)cache;
	}

	CUtlVector< int > boneMap;				// maps global bone to local bone
	CUtlVector< int > masterBone;			// maps local bone to global bone
	CUtlVector< int > masterSeq;			// maps local sequence to master sequence
	CUtlVector< int > masterAnim;			// maps local animation to master animation
	CUtlVector< int > masterAttachment;	// maps local attachment to global
	CUtlVector< int > masterPose;			// maps local pose parameter to global
	CUtlVector< int > masterNode;			// maps local transition nodes to global
};

struct virtualsequence_t
{
	int	flags;
	int activity;
	int group;
	int index;
};

struct virtualgeneric_t
{
	int group;
	int index;
};

struct virtualmodel_t
{
	char pad_mutex[0x8];
	CUtlVector< virtualsequence_t > m_seq;
	CUtlVector< virtualgeneric_t > m_anim;
	CUtlVector< virtualgeneric_t > m_attachment;
	CUtlVector< virtualgeneric_t > m_pose;
	CUtlVector< virtualgroup_t > m_group;
	CUtlVector< virtualgeneric_t > m_node;
	CUtlVector< virtualgeneric_t > m_iklock;
	CUtlVector< unsigned short > m_autoplaySequences;
};

class c_studio_hdr
{
public:
	mutable studiohdr_t* m_pStudioHdr; //0x0000 
	mutable virtualmodel_t* m_pVModel; //0x0004 
	char				pad_unknown[0x4]; //0x0008 
	mutable CUtlVector< const studiohdr_t* > m_pStudioHdrCache; //0x000C 
	mutable int			m_nFrameUnlockCounter; //0x0020 
	int* m_pFrameUnlockCounter; //0x0024 
	char				pad_mutex[0x8]; //0x0028 
	CUtlVector< int >	m_boneFlags; //0x0030
	CUtlVector< int >	m_boneParent; //0x0050 
	inline int			numbones(void) const { return m_pStudioHdr->numbones; };
	inline mstudiobone_t* pBone(int i) const { return m_pStudioHdr->pBone(i); };
	inline virtualmodel_t* GetVirtualModel(void) const { return m_pVModel; };
	int	GetNumAttachments(void) const
	{
		if (m_pStudioHdr->numincludemodels == 0)
		{
			return m_pStudioHdr->numlocalattachments;
		}

		virtualmodel_t* pVModel = GetVirtualModel();
		Assert(pVModel);

		return pVModel->m_attachment.Count();
	}
	const studiohdr_t* GroupStudioHdr(int i)
	{
		const studiohdr_t* pStudioHdr = m_pStudioHdrCache[i];

		if (!pStudioHdr)
		{
			virtualgroup_t* pGroup = &m_pVModel->m_group[i];
			pStudioHdr = pGroup->GetStudioHdr();
		}

		Assert(pStudioHdr);
		return pStudioHdr;
	}
	const mstudioattachment_t& pAttachment(int i)
	{
		if (m_pVModel == NULL)
		{
			return *m_pStudioHdr->pLocalAttachment(i);
		}

		Assert(m_pVModel);

		const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_attachment[i].group);

		return *pStudioHdr->pLocalAttachment(m_pVModel->m_attachment[i].index);
	}
	int	GetAttachmentBone(int i)
	{
		const mstudioattachment_t& attachment = pAttachment(i);

		// remap bone
		virtualmodel_t* pVModel = GetVirtualModel();
		if (pVModel)
		{
			virtualgroup_t* pGroup = &pVModel->m_group[pVModel->m_attachment[i].group];
			int iBone = pGroup->masterBone[attachment.localbone];
			if (iBone == -1)
				return 0;
			return iBone;
		}
		return attachment.localbone;
	}

}; //Size=0x0054
