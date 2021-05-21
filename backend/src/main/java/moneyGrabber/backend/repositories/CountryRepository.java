package moneyGrabber.backend.repositories;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
import moneyGrabber.backend.models.Country;

@Repository
public interface CountryRepository extends JpaRepository<Country, Long> {

}
