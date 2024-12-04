/*

Queries used for Tableau Project

*/



-- 1. 

Select SUM(new_cases) as total_cases, SUM(cast(new_deaths as float)) as total_deaths, SUM(cast(new_deaths as float))/SUM(New_Cases)*100 as DeathPercentage
From `coviddeaths (1)`
-- Where location like '%states%'
where continent is not null 
-- Group By date
order by 1,2;

-- Just a double check based off the data provided
-- numbers are extremely close so we will keep them - The Second includes "International"  Location


-- Select SUM(new_cases) as total_cases, SUM(cast(new_deaths as int)) as total_deaths, SUM(cast(new_deaths as int))/SUM(New_Cases)*100 as DeathPercentage
-- From PortfolioProject..CovidDeaths
-- --Where location like '%states%'
-- where location = 'World'
-- --Group By date
-- order by 1,2


-- 2. 

-- We take these out as they are not inluded in the above queries and want to stay consistent
-- European Union is part of Europe

Select location, SUM(cast(new_deaths as float)) as TotalDeathCount
From `coviddeaths (1)`
-- Where location like '%states%'
Where -- continent is null 
location in ('Asia', 'Europe', 'North America', 'South America', 'Africa', 'Australia')
Group by location
order by TotalDeathCount desc;


-- 3.

Select Location, Population, MAX(total_cases) as HighestInfectionCount,  Max((total_cases/population))*100 as PercentPopulationInfected
From `coviddeaths (1)`
-- Where location like '%states%'
Group by Location, Population
order by PercentPopulationInfected desc;


-- 4.


Select Location, Population,date, MAX(total_cases) as HighestInfectionCount,  Max((total_cases/population))*100 as PercentPopulationInfected
From `coviddeaths (1)`
-- Where location like '%states%'
Group by Location, Population, date
order by PercentPopulationInfected desc;




-- ----------------------------------------------------------
-- Starts here


select * from `coviddeaths (1)` 
where continent is not null
order by 3,4;
-- select * from `covidvaccinations` order by 3,4;
select Location, date, total_cases, new_cases, total_deaths, population
from `coviddeaths (1)` order by Location, date desc;

-- Likelyhood of dying if you get covid
select Location, date, total_cases, total_deaths, (total_deaths/total_cases)*100 as Death_percentage
from `coviddeaths (1)` where location like 'india' 
order by Location, date desc;

-- Looking at total cases vs population
select Location, date, total_cases, population, (total_cases/population)*100 as percent_of_population
from `coviddeaths (1)` where location like 'india' 
order by 5 desc;

-- Looking at countries with higest infection rate
SET sql_mode = (SELECT REPLACE(@@sql_mode, 'ONLY_FULL_GROUP_BY', ''));
select Location, population, max(total_cases) as Highestinfectioncount, max((total_cases/population))*100 as Affected_percentage
from `coviddeaths (1)` 
group by Location, population
order by 4 desc;

-- Hihest death count per population
select Location, max(total_deaths) as totaldeathcount
from `coviddeaths (1)` 
group by Location
order by 2 desc;

-- BY continent
select continent, max(total_deaths) as totaldeathcount
from `coviddeaths (1)` 
where continent is not null 
group by continent
order by 2 desc;

-- Global numbers
select date, sum(new_cases) as daily_cases, sum(cast(new_deaths as float)) as daily_deaths, total_deaths, (sum(cast(new_deaths as float))/ sum(new_cases))*100 as daily_Death_percentage
from `coviddeaths (1)` 
where continent is not null 
group by date 
order by date desc;

-- ------------------------
-- Both Tables
select death.continent, death.location, death.population, death.date, 
vaccine.new_vaccinations, sum(vaccine.new_vaccinations) over (partition by death.location order by death.location, death.date)
as rolling_vaccinations
from `coviddeaths (1)` death
join covidvaccinations vaccine
 on death.location = vaccine.location
 and death.date = vaccine.date
where death.continent is not null
order by 2,4;
 
 
 -- -----------------------
with popvsvacc (continent, location, population, date, new_vaccinations, rolling_vaccinations)
as 
(
select death.continent, death.location, death.population, death.date, 
vaccine.new_vaccinations, sum(vaccine.new_vaccinations) over (partition by death.location order by death.location, death.date)
as rolling_vaccinations
from `coviddeaths (1)` as death
join covidvaccinations as vaccine
 on death.location = vaccine.location
 and death.date = vaccine.date
where death.continent is not null
order by 2,4
)
select *, (rolling_vaccinations/population)*100 from popvsvacc;

-- ---------------------------
-- second way temp table
-- Temp Table
drop table if exists percentpopulationvaccinated;
create table percentpopulationvaccinated
(
continent varchar(255),
location varchar(255),
population numeric,
date date,
new_vaccinations numeric,
rolling_vaccinations numeric
);

insert into percentpopulationvaccinated
select death.continent, death.location, death.population, death.date, 
vaccine.new_vaccinations, sum(vaccine.new_vaccinations) over (partition by death.location order by death.location, death.date)
as rolling_vaccinations
from `coviddeaths (1)` as death
join covidvaccinations as vaccine
 on death.location = vaccine.location
 and death.date = vaccine.date
where death.continent is not null
order by 2,4;

select * from percentpopulationvaccinated;

-- -------------------
-- Creating view to store data and also a third way

create view percentpopulationvaccinated1 as
select death.continent, death.location, death.population, death.date, 
vaccine.new_vaccinations, sum(vaccine.new_vaccinations) over (partition by death.location order by death.location, death.date)
as rolling_vaccinations
from `coviddeaths (1)` as death
join covidvaccinations as vaccine
 on death.location = vaccine.location
 and death.date = vaccine.date
where death.continent is not null
order by 2,4;

