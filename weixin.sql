BEGIN;
CREATE TABLE `weixin_user` (
    `uid` integer AUTO_INCREMENT NOT NULL PRIMARY KEY,
    `pwd` varchar(32) NOT NULL,
    `name` varchar(32) NOT NULL,
    `timestamp` datetime NOT NULL
)
;

CREATE TABLE `weixin_friend` (
    `uid` integer NOT NULL,
    `fid` integer NOT NULL
)
;
ALTER TABLE `weixin_friend` ADD PRIMARY KEY (`uid`, `fid`);
ALTER TABLE `weixin_friend` ADD CONSTRAINT `weixin_friend_fk_uid` FOREIGN KEY (`uid`) REFERENCES `weixin_user` (`uid`);
ALTER TABLE `weixin_friend` ADD CONSTRAINT `weixin_friend_fk_fid` FOREIGN KEY (`fid`) REFERENCES `weixin_user` (`uid`);

CREATE TABLE `weixin_group` (
    `gid` integer AUTO_INCREMENT NOT NULL PRIMARY KEY,
    `uid` integer NOT NULL,
    `name` varchar(32) NOT NULL
)
;
ALTER TABLE `weixin_group` ADD CONSTRAINT `weixin_group_fk_uid` FOREIGN KEY (`uid`) REFERENCES `weixin_user` (`uid`);

CREATE TABLE `weixin_user_group` (
    `uid` integer NOT NULL,
    `gid` integer NOT NULL
)
;
ALTER TABLE `weixin_user_group` ADD PRIMARY KEY (`uid`, `gid`);
ALTER TABLE `weixin_user_group` ADD CONSTRAINT `weixin_user_group_fk_uid` FOREIGN KEY (`uid`) REFERENCES `weixin_user` (`uid`);
ALTER TABLE `weixin_user_group` ADD CONSTRAINT `weixin_user_group_fk_gid` FOREIGN KEY (`gid`) REFERENCES `weixin_group` (`gid`);

CREATE TABLE `weixin_usermsg` (
    `mid` integer AUTO_INCREMENT NOT NULL PRIMARY KEY,
    `uid` integer NOT NULL,
    `fid` integer NOT NULL,
    `content` longtext NOT NULL,
    `timestamp` datetime NOT NULL
)
;
ALTER TABLE `weixin_usermsg` ADD CONSTRAINT `weixin_usermsg_fk_uid` FOREIGN KEY (`uid`) REFERENCES `weixin_user` (`uid`);
ALTER TABLE `weixin_usermsg` ADD CONSTRAINT `weixin_usermsg_fk_fid` FOREIGN KEY (`fid`) REFERENCES `weixin_user` (`uid`);

CREATE TABLE `weixin_groupmsg` (
    `mid` integer AUTO_INCREMENT NOT NULL PRIMARY KEY,
    `uid` integer NOT NULL,
    `gid` integer NOT NULL,
    `content` longtext NOT NULL,
    `timestamp` datetime NOT NULL
)
;
ALTER TABLE `weixin_groupmsg` ADD CONSTRAINT `weixin_groupmsg_fk_uid` FOREIGN KEY (`uid`) REFERENCES `weixin_user` (`uid`);
ALTER TABLE `weixin_groupmsg` ADD CONSTRAINT `weixin_groupmsg_fk_gid` FOREIGN KEY (`gid`) REFERENCES `weixin_group` (`gid`);

-- 索引先不管, 有时间再说
-- CREATE INDEX `weixin_group_2600da4b` ON `weixin_group` (`uid`);
-- CREATE INDEX `weixin_usermsg_2600da4b` ON `weixin_usermsg` (`uid`);
-- CREATE INDEX `weixin_usermsg_44920622` ON `weixin_usermsg` (`fid`);
-- CREATE INDEX `weixin_message_2600da4b` ON `weixin_message` (`uid`);
-- CREATE INDEX `weixin_groupmsg_2600da4b` ON `weixin_groupmsg` (`uid`);
-- CREATE INDEX `weixin_groupmsg_daae703` ON `weixin_groupmsg` (`gid`);
-- CREATE INDEX `weixin_groupmsg_4b72d7bd` ON `weixin_groupmsg` (`mid`);

