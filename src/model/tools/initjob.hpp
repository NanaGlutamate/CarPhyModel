#pragma once

namespace carphymodel {

struct DoJobOnConstruct{
    template<typename Job, typename ...Params>
    DoJobOnConstruct(Job&& j, Params&& ...params){std::invoke(std::forward<Job>(j), std::forward<Params>(params)...);};
};

}