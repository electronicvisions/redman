#pragma once

namespace redman {

struct Policy
{
};

/** Switching mode for enable/disable functionality of resources and black/whitelists.
 *  The modes mark different modes of operation (for enable/disable functionality) with
 *  respect to "errors".
 *  \note C++98-style of a scoped enum due to Py++ reasons.
 */
struct switch_mode {
	 enum type { THROW, NONTHROW };
};

} // redman
