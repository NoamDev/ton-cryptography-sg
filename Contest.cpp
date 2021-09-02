#include <tvm/contract.hpp>
#include <tvm/contract_handle.hpp>
#include <tvm/default_support_functions.hpp>
#include <tvm/emit.hpp>
#include <tvm/smart_switcher.hpp>

#include "MultiBallot.hpp"
#include "ProposalRoot.hpp"

using namespace tvm;
using namespace schema;

static constexpr unsigned TIMESTAMP_DELAY = 1800;
using replay_protection_t = replay_attack_protection::timestamp<TIMESTAMP_DELAY>;

__interface [[no_pubkey]] IContest {
    [[internal, noaccept, dyn_chain_parse]] void constructor(
        bytes title, bytes link, uint256 hash, address juryAddr, dict_array<uint256> juryKeys, uint64 startsIn,
        uint64 lastsFor, uint64 votingWindow, uint256 sendApprovalGrams) = 777;

    [[internal, noaccept]] void sendApproval();

    // ============== getters ==============
    [[getter]] bytes getTitle();

    [[getter]] bytes getLink();

    [[getter]] uint256 getHash();

    [[getter]] address getJuryAddr();

    [[getter]] dict_array<uint256> getJuryKeys();

    [[getter]] uint64 getStartsIn();

    [[getter]] uint64 getLastsFor();

    [[getter]] uint64 getVotingWindow();
};

struct DContest {
    bytes title_;
    bytes link_;
    uint256 hash_;
    address juryAddr_;
    dict_array<uint256> juryKeys_;
    uint64 startsIn_;
    uint64 lastsFor_;
    uint64 votingWindow_;
    handle<IProposalRoot> proposal_;
    Grams sendApprovalGrams_;
};

__interface EContest {};

class Contest final : public smart_interface<IContest>, public DContest {
public:
    __always_inline void constructor(bytes title, bytes link, uint256 hash, address juryAddr,
                                     dict_array<uint256> juryKeys, uint64 startsIn, uint64 lastsFor,
                                     uint64 votingWindow, uint256 sendApprovalGrams) {
        title_ = title;
        link_ = link;
        hash_ = hash;
        juryAddr_ = juryAddr;
        juryKeys_ = juryKeys;
        startsIn_ = startsIn;
        lastsFor_ = lastsFor;
        votingWindow_ = votingWindow;
        proposal_ = handle<IProposalRoot> {int_sender()};
        sendApprovalGrams_ = sendApprovalGrams.get();
    }

    __always_inline void sendApproval() {
        proposal_(sendApprovalGrams_).contestApproved();
    }

    // ========== getters ==========
    __always_inline bytes getTitle() {
        return title_;
    }
    __always_inline bytes getLink() {
        return link_;
    }
    __always_inline uint256 getHash() {
        return hash_;
    }
    __always_inline address getJuryAddr() {
        return juryAddr_;
    }
    __always_inline dict_array<uint256> getJuryKeys() {
        return juryKeys_;
    }
    __always_inline uint64 getStartsIn() {
        return startsIn_;
    }
    __always_inline uint64 getLastsFor() {
        return lastsFor_;
    }
    __always_inline uint64 getVotingWindow() {
        return votingWindow_;
    }

public:
    // ==================== Support methods =========================== //
    // default processing of unknown messages
    __always_inline static int _fallback(cell msg, slice msg_body) {
        return 0;
    }
    DEFAULT_SUPPORT_FUNCTIONS(IContest, replay_protection_t);
};

DEFINE_JSON_ABI(IContest, DContest, EContest);

// ----------------------------- Main entry functions ---------------------- //
DEFAULT_MAIN_ENTRY_FUNCTIONS(Contest, IContest, DContest, TIMESTAMP_DELAY)

