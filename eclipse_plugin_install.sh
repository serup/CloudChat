#!/usr/bin/env bash -l

echo "installing plugins into already installed eclipse - please wait it takes long time..."
eclipse \
-clean -purgeHistory \
-application org.eclipse.equinox.p2.director \
-noSplash \
-repository \
http://download.eclipse.org/modeling/tmf/xtext/updates/composite/releases/,\
http://download.eclipse.org/releases/juno,\
http://download.eclipse.org/windowbuilder/WB/release/R201406251200/4.2/,\
http://download.eclipse.org/technology/swtbot/helios/dev-build/update-site,\
http://update.atlassian.com/atlassian-eclipse-plugin/e3.8 \
-installIUs \
org.eclipse.xpand.sdk.feature.group,\
org.eclipse.swtbot.eclipse.feature.group,\
org.eclipse.swtbot.eclipse.test.junit4.feature.group,\
org.eclipse.swtbot.feature.group,\
org.eclipse.swtbot.forms.feature.group,\
org.eclipse.swtbot.ide.feature.group,\
org.eclipse.egit.mylyn.feature.group,\
org.eclipse.mylyn.bugzilla_feature.feature.group,\
org.eclipse.mylyn.builds.feature.group,\
org.eclipse.mylyn.context_feature.feature.group,\
org.eclipse.mylyn.discovery.feature.group,\
org.eclipse.mylyn.gerrit.feature.feature.group,\
org.eclipse.mylyn.git.feature.group,\
org.eclipse.mylyn.github.feature.feature.group,\
org.eclipse.mylyn.hudson.feature.group,\
org.eclipse.mylyn.ide_feature.feature.group,\
org.eclipse.mylyn.java_feature.feature.group,\
org.eclipse.mylyn.monitor.feature.group,\
org.eclipse.mylyn.pde_feature.feature.group,\
org.eclipse.mylyn.tasks.ide.feature.group,\
org.eclipse.mylyn.team_feature.feature.group,\
org.eclipse.mylyn.trac_feature.feature.group,\
org.eclipse.mylyn.wikitext_feature.feature.group,\
org.eclipse.mylyn_feature.feature.group,\
org.eclipse.emf.cdo.sdk.feature.group,\
org.eclipse.emf.query.sdk.feature.group,\
com.atlassian.connector.eclipse.jira.feature.group \
-vmargs -Declipse.p2.mirrors=true -Djava.net.preferIPv4Stack=true

echo "- DONE"

