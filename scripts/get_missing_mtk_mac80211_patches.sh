#!/bin/sh
find ../mtk-openwrt-feeds/autobuild_mac80211_release/package/kernel/mac80211/patches/subsys/ ! -type d | \
	xargs -n1 basename | \
	xargs -I{} bash -c '(git ls-files package/kernel/mac80211/patches/subsys | grep -iq $(echo {} | cut -d- -f5- | rev | cut -d- -f1- | rev | sed -e "s|[-_]|.|g"); ) || echo {}' | \
	sort -n | \
	grep -E -ve '^(bp-|[0-9]+)' \
		-ve '(correct-legacy-rates-check-in-ieee80211|add-s1g-category-to-_ieee80211_is_robus)' \
		-ve '(add-DFS-CAC-countdown-in-CSA-flow|send-deauth-frame-if-CAC-is-required-du)' \
		-ve '(Add-cert-mode-for-ba-timeout-workaround|backport-refactor-STA-CSA-paring-flow)' \
		-ve '(mask-kernel-version-limitation-and-fill|add-fill-receive-path-ops-to-get-wed-id)' \
		-ve '(add-support-for-letting-drivers-registe)'
